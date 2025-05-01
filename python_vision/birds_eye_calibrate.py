import cv2
import numpy as np
import math

PI = 3.1415926


def update_perspective(val):
    alpha = (cv2.getTrackbarPos("Alpha", "Result") - 90) * PI / 180
    beta = (cv2.getTrackbarPos("Beta", "Result") - 90) * PI / 180
    gamma = (cv2.getTrackbarPos("Gamma", "Result") - 90) * PI / 180
    dist = .355565
    image_size = (frameWidth*2, frameHeight*2)
    w, h = image_size
    h = cv2.getTrackbarPos("Height", "Result") * 2
    w = cv2.getTrackbarPos("Width", "Result") * 2

    A1 = np.array([[1, 0, -w],
                   [0, 1, -h],
                   [0, 0, 1]], dtype=np.float32)  # Adjusted to 3x3 matrix

    RX = np.array([[1, 0, 0],
                   [0, math.cos(alpha), -math.sin(alpha)],
                   [0, math.sin(alpha), math.cos(alpha)]], dtype=np.float32)

    RY = np.array([[math.cos(beta), 0, -math.sin(beta)],
                   [0, 1, 0],
                   [math.sin(beta), 0, math.cos(beta)]], dtype=np.float32)

    RZ = np.array([[math.cos(gamma), -math.sin(gamma), 0],
                   [math.sin(gamma), math.cos(gamma), 0],
                   [0, 0, 1]], dtype=np.float32)

    R = np.dot(np.dot(RX, RY), RZ)
    R = np.asarray([[1, 0, 0], [0, 0, 1], [0, -1, 0]])

    T = np.array([[1, 0, 0],
                   [0, dist, 0],
                   [0, 0, 1]], dtype=np.float32)  # Adjusted to 3x3 matrix

    K = np.array([
        [603.47021484375, 0, 326.07391357421875],
        [0, 603.1049194335938, 235.4612274169922],
        [0, 0, 1]
    ])

    transformationMat = np.dot(np.dot(np.dot(K, T), R), A1)

    # Extract the top-left 3x3 portion of the transformation matrix for warpPerspective
    transformationMat = transformationMat[:3, :3]
    transformationMat = np.linalg.inv(transformationMat)
    destination = cv2.warpPerspective(source, transformationMat, image_size)
    print(destination)
    cv2.imshow("Result", destination)

source = cv2.imread('test.png')  # Replace with your image file path
frameHeight, frameWidth, channels = source.shape
cv2.namedWindow("Result", cv2.WINDOW_NORMAL)
cv2.createTrackbar("Alpha", "Result", 90, 180, update_perspective)
cv2.createTrackbar("Beta", "Result", 90, 180, update_perspective)
cv2.createTrackbar("Gamma", "Result", 90, 180, update_perspective)
cv2.createTrackbar("Height", "Result", 0, frameHeight -1, update_perspective)
cv2.createTrackbar("Width", "Result", 0, frameWidth -1, update_perspective)

# Initialize the perspective before entering the loop
update_perspective(0)

while True:
    update_perspective(0)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.waitKey(0)
cv2.destroyAllWindows()
