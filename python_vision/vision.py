import cv2
import numpy as np
import util

def get_cones(frame):
    dist_orange = util.dist_to_rgb_weighted(
        frame,
        255, 0, 0,
        0.5, 0.25, 0.25
    )
    dist_orange = util.invert(dist_orange)
    threshold = util.hysteresis_threshold(dist_orange, 120, 160)
    return threshold

def get_track(frame, threshold_val: int=10):
    # Calculates the distance from every pixel to its grayscale.
    dist_mean = util.dist_to_mean(frame).astype(np.uint8)
    # Track is very gray so distance should be small
    threshold = util.threshold(dist_mean, threshold_val)
    threshold = util.invert(threshold)
    # Erode then dialte the image to remove noisy connections from track to
    # outside
    opened = util.opening(threshold, 7)
    # Floodfill from the bottommost pixel at half the width
    flood = util.flood_fill(opened, 0.5, 1)
    return flood