import numpy as np
import cv2

# Absolute [Y, X] pixel coordinates of each grid corner where [0, 0] is top-left
corner_pixel_coordinates = np.array([
    [257, 363], [199, 389], [104, 432],
    [356, 368], [321, 400], [258, 458],
    [467, 371], [466, 405], [465, 471],
    [576, 371], [611, 403], [673, 462],
    [677, 368], [732, 394], [823, 439],
    [759, 362], [827, 384], [924, 416],
    # [830, 357], [893, 374]
], dtype=np.float32).reshape(-1, 1, 2)  # Reshape for OpenCV compatibility

# Grid coordinates where top left is [0, 0, Z=0]
corner_grid_coordinates = np.array([
    [0, 0, 0], [1, 0, 0], [2, 0, 0],
    [0, 1, 0], [1, 1, 0], [2, 1, 0],
    [0, 2, 0], [1, 2, 0], [2, 2, 0],
    [0, 3, 0], [1, 3, 0], [2, 3, 0],
    [0, 4, 0], [1, 4, 0], [2, 4, 0],
    [0, 5, 0], [1, 5, 0], [2, 5, 0],
    # [0, 6, 0], [1, 6, 0]
], dtype=np.float32).reshape(-1, 1, 3)  # Reshape for OpenCV compatibility

image_width = 906
image_height = 540

def calibrate_camera():
    """Calibrate the camera using known object points and image points."""
    ret, matrix, dist, rvecs, tvecs = cv2.calibrateCamera(
        [corner_grid_coordinates],  # Object points
        [corner_pixel_coordinates],  # Image points
        (image_width, image_height),  # Image size (width, height)
        None, None
    )
    if not ret:
        raise RuntimeError("Camera calibration failed!")
    
    return ret, matrix, dist, rvecs, tvecs

def get_undistort_function():
    """Returns a function that undistorts an image using calibration data."""
    ret, matrix, dist, rvecs, tvecs = calibrate_camera()

    new_camera_matrix, roi = cv2.getOptimalNewCameraMatrix(
        matrix, dist, (image_width, image_height), 1, (image_width, image_height)
    )
    
    mapx, mapy = cv2.initUndistortRectifyMap(
        matrix, dist, None, new_camera_matrix, (image_width, image_height), cv2.CV_16SC2
    )

    def undistort(frame):
        """Apply undistortion to a given frame."""
        return cv2.remap(frame, mapx, mapy, cv2.INTER_LINEAR)

    return undistort

def draw_grid_corners(image):
    """Draws the corner pixel coordinates on a copy of the input image and returns it."""
    # Make a copy of the image to avoid modifying the original
    output_image = image.copy()

    # Iterate through each coordinate
    for i in range(corner_pixel_coordinates.shape[0]):
        y, x = corner_pixel_coordinates[i][0]
        y, x = int(y), int(x)
        # Draw the red dot at the corner
        cv2.circle(output_image, (x, y), 5, (0, 0, 255), -1)  # Red dot
        
        # Label the dot with the index
        cv2.putText(output_image, str(i), (x + 5, y - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)

    return output_image  # Return the modified image

def test(frame):
    cv2.drawChessboardCorners(frame, (3, 6), corner_pixel_coordinates, True)