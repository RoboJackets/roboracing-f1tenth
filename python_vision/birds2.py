# https://lacie-life.github.io/posts/BEV/
import numpy as np
import cv2

# === Step 1: Set your camera intrinsic matrix ===
K = np.array([
    [603.47021484375, 0.0, 326.07391357421875],
    [0.0, 603.1049194335938, 235.4612274169922],
    [0.0, 0.0, 1.0]
])

# === Step 2: Define extrinsic parameters (camera facing forward, 14in above ground) ===
# Convert 14 inches to meters
camera_height_m = 14 * 0.0254

# Since the camera is forward-facing, we rotate it -90° about X to get a top-down view
R = np.array([
    [1, 0, 0],
    [0, 0, -1],
    [0, 1, 0]
])
t = np.array([[0], [-camera_height_m], [0]])

# Compose the 3x4 projection matrix [R|t]
RT = np.hstack((R, t))

# === Step 3: Compute homography from ground plane to image plane ===
H = K @ RT  # 3x3 homography for z=0 plane

# === Step 4: Define BEV image scale and bounds (in meters) ===
bev_x_range = (0, 20)   # forward (meters)
bev_y_range = (-6, 6)  # left to right (meters)

meters_per_pixel = 0.05  # 2cm per pixel
bev_width = int((bev_y_range[1] - bev_y_range[0]) / meters_per_pixel)
bev_height = int((bev_x_range[1] - bev_x_range[0]) / meters_per_pixel)

# === Step 5: Create a mesh grid of BEV pixel coordinates in world space ===
ys = np.linspace(bev_y_range[0], bev_y_range[1], bev_width)
xs = np.linspace(bev_x_range[0], bev_x_range[1], bev_height)
Y, X = np.meshgrid(ys, xs)  # shape: (H, W)

# Flatten grid and create homogeneous world coordinates (z=0)
world_coords = np.stack([X.ravel(), Y.ravel(), np.zeros_like(X.ravel()), np.ones_like(X.ravel())])

# Project to image coordinates using homography
uv = H @ world_coords[:4, :]
uv = uv[:2] / uv[2]

# Reshape to (H, W, 2) map
map_x = uv[0].reshape(bev_width, bev_height).astype(np.float32)
map_y = uv[1].reshape(bev_width, bev_height).astype(np.float32)

# === Step 6: Apply remap using OpenCV ===
input_img = cv2.imread("test.png")  # Your forward-facing image
bev_img = cv2.remap(input_img, map_x, map_y, interpolation=cv2.INTER_LINEAR, borderMode=cv2.BORDER_CONSTANT)

# Rotate and flip to make it easier to interpret
bev_img = cv2.rotate(bev_img, cv2.ROTATE_90_CLOCKWISE)

# === Step 7: Save or show result ===
cv2.imwrite("bev_output.jpg", bev_img)
# cv2.imshow("BEV View", bev_img); cv2.waitKey(0)
