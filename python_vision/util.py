import cv2
import numpy as np

# Computes the distance from all pixels to the target
def dist_to_hsv_weighted(
    img,
    target_hue: int,
    target_sat: int,
    target_val: int,
    hue_weight: float=0.7,
    sat_weight: float=0.15,
    val_weight: float=0.15,
):
    # Convert BGR to HSV color space
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    # Split HSV channels
    H, S, V = cv2.split(hsv)
    H = H.astype(np.float32) * 2
    S = S.astype(np.float32)
    V = V.astype(np.float32)

    diff_H = np.absolute(H - target_hue)
    dH = hue_weight * np.minimum(diff_H, (-1) * diff_H + 360) 
    dS = sat_weight * (S - target_sat)
    dV = val_weight * (V - target_val)

    dX = dS * np.cos(dH)
    dY = dS * np.sin(dH)
    dZ = dV

    # dist_squared = hue_weight * (X - org_x)**2 + (Y - org_y)**2 + (Z - org_z)**2
    dist_squared = dX**2 + dY**2 + dZ**2
    dist = np.sqrt(dist_squared)
    np.minimum(dist, 255)
    dist = dist.astype(np.uint8)

    return dist

# Computes the distance from all pixels to the target
def dist_to_rgb_weighted(
    img,
    target_r: int,
    target_g: int,
    target_b: int,
    r_weight: float=1/3,
    g_weight: float=1/3,
    b_weight: float=1/3,
):
    b, g, r = cv2.split(img)
    b = b.astype(np.int32)
    g = g.astype(np.int32)
    r = r.astype(np.int32)

    dr = r - target_r
    dg = g - target_g
    db = b - target_b

    dist_squared = r_weight * dr**2 + g_weight * dg**2 + b_weight * db**2
    dist = np.sqrt(dist_squared)
    np.minimum(dist, 255)
    return dist.astype(np.uint8)


# Applies a binary threshold to a grayscale image
def threshold(img, threshold: float):
    ret, thresh = cv2.threshold(img, threshold, 255, cv2.THRESH_BINARY)
    return thresh

# Inverts a binary image
def invert(img):
    max_val = np.iinfo(img.dtype).max
    return max_val - img

# Applies a hysteresis threshold to a grayscale image, connecting to white (255)
def hysteresis_threshold(img, lower_threshold: float, upper_threshold: float):
    # Identify strong and weak pixels
    strong_pixels = (img >= upper_threshold).astype(np.uint8)  
    weak_pixels = ((img >= lower_threshold) & (img < upper_threshold)).astype(np.uint8)

    # Connect weak pixels to strong ones using morphological dilation
    kernel = np.ones((3, 3), np.uint8)  # 3x3 kernel for connectivity check
    connected = cv2.dilate(strong_pixels, kernel, iterations=3)

    # Keep weak pixels that are connected to strong ones
    final_mask = np.where((connected == 1) & (weak_pixels == 1), 1, strong_pixels)

    return (final_mask * 255).astype(np.uint8)  # Convert back to 0-255 range

def avg_pixel_close_to_hsv(
    img,
    target_hue: int,
    target_sat: int,
    target_val: int,
    max_dist: int,
    hue_weight: float=0.7,
    sat_weight: float=0.15,
    val_weight: float=0.15,
):
    dist = dist_to_hsv_weighted(
        img, target_hue, target_sat, target_val,
        hue_weight, sat_weight, val_weight
    )
    mask = (dist <= max_dist).astype(np.uint8)
    mean_bgr = cv2.mean(img, mask)
    mean_bgr_img = np.uint8([[mean_bgr]])
    mean_hsv = cv2.cvtColor(mean_bgr_img, cv2.COLOR_BGR2HSV)
    return mean_hsv[0][0]

def avg_pixel_close_to_rgb(
    img,
    target_r: int,
    target_g: int,
    target_b: int,
    max_dist: int,
    r_weight: float=1/3,
    g_weight: float=1/3,
    b_weight: float=1/3,
):
    dist = dist_to_rgb_weighted(
        img, target_r, target_g, target_b,
        r_weight, g_weight, b_weight
    )
    mask = (dist <= max_dist).astype(np.uint8)
    mean_bgr = cv2.mean(img, mask)
    mean_rgb = mean_bgr[::-1]
    return mean_rgb

def test(img):
    for row in range(100, 200):
        for col in range(400, 700):
            img[row][col] = np.asarray([0, 0, 0], dtype=np.uint8)

# Computes the average rgb value of a pixel in the rectangle from
# [x_start, y_start] to (x_end, y_end).
# Top left is [0, 0], and bottom right is (width, height)
# An additional filter function that returns true or false for each pixel
# can be passed, such that the average is only computed for pixels that
# return true.
def avg_pixel_in_area(
    img,
    x_start: int | float,
    x_end: int | float,
    y_start: int | float,
    y_end: int | float,
    filter=None,
    *args
):
    img_height, img_width = img.shape[0:2]
    # Convert 0..1 positions to absolute
    if x_start < 1:
        x_start = int(x_start * img_width)
    if x_end <= 1:
        x_end = int(x_end * img_width)
    if y_start < 1:
        y_start = int(y_start * img_height)
    if y_end <= 1:
        y_end = int(y_end * img_height)
    # Get the rectangle and compute average
    rect = img[y_start:y_end, x_start:x_end]
    if filter is None:
        return np.mean(rect, axis=(0, 1))
    else:
        mask = filter(rect, *args)
        # return np.mean(rect, axis=(0, 1), where=mask)
        return cv2.mean(rect, mask.astype(np.uint8))

# Returns a mask of True for all pixels whose rgb/gray values fall between
# the given, inclusive range
def in_range(
    img,
    min_pix: int | np.ndarray ,
    max_pix: int | np.ndarray,
):
    # Grayscale image
    if img.ndim == 2:
        return (img >= min_pix) & (img <= max_pix)
    # Colour image
    min_mask = (img >= min_pix).all(axis=-1)
    max_mask = (img <= max_pix).all(axis=-1)
    return min_mask & max_mask
    

def rect(
    img,
    x_start: int | float,
    x_end: int | float,
    y_start: int | float,
    y_end: int | float, 
):
    img_height, img_width = img.shape[0:2]
    # Convert 0..1 positions to absolute
    if x_start < 1:
        x_start = int(x_start * img_width)
    if x_end <= 1:
        x_end = int(x_end * img_width)
    if y_start < 1:
        y_start = int(y_start * img_height)
    if y_end <= 1:
        y_end = int(y_end * img_height)
    
    rect = img[y_start:y_end, x_start:x_end]
    return rect

def draw_rect(
    img,
    x_start: int | float,
    x_end: int | float,
    y_start: int | float,
    y_end: int | float,
    filter=None,
    *args
):
    img_height, img_width = img.shape[0:2]
    # Convert 0..1 positions to absolute
    if x_start < 1:
        x_start = int(x_start * img_width)
    if x_end <= 1:
        x_end = int(x_end * img_width)
    if y_start < 1:
        y_start = int(y_start * img_height)
    if y_end <= 1:
        y_end = int(y_end * img_height)
    
    rect = img[y_start:y_end, x_start:x_end] 
    mask = filter(rect, *args)
    rect[mask] = np.asarray([0, 0, 0], np.uint8)

# Returns the mean of each pixel using the given weights as a 2d f64 array
def mean(img, bgr_weights: np.ndarray=None):
    return np.average(img, axis=2, weights=bgr_weights)

# Returns the distance of each pixel to its mean
# Note that this distance can be greater than 255, unless cap255 is set to true
def dist_to_mean(
    img,
    bgr_weights: np.ndarray=None,
    distance_scale: float=1, cap255: bool=False
):
    avg = mean(img, bgr_weights)
    diff = img - avg[:, :, np.newaxis]
    dist = np.linalg.norm(diff, axis=2) * distance_scale
    if cap255:
        np.minimum(dist, 255)
    return dist
    