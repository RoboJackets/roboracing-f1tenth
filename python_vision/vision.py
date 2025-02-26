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