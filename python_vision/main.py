import cv2
import numpy as np
import util

def main():
    # Open webcam (0 = default camera)
    cap = cv2.VideoCapture("lowres.MP4")
    cv2.namedWindow("Original", cv2.WINDOW_NORMAL)
    # cv2.namedWindow("Filter", cv2.WINDOW_NORMAL)

    while True:
        ret, frame = cap.read()
        if not ret:
            continue  # If the camera fails, exit

        # Remove above horizon
        frame = util.rect(frame, 0, 1, 0.48, 1)

        possible_road = util.in_range(
            frame,
            np.asarray([80, 80, 80], dtype=np.uint8),
            np.asarray([160, 160, 160], dtype=np.uint8)
        )

        dist_mean = util.dist_to_mean(frame, cap255=True).astype(np.uint8)
        
        threshold = util.threshold(dist_mean, 10)

        # min_pix = np.asarray([80, 80, 80], dtype=np.uint8)
        # max_pix = np.asarray([170, 170, 170], dtype=np.uint8)
        # x_lims = (0.15, 0.85)
        # y_lims = (0.3, 1)

        # avg = util.avg_pixel_in_area(
        #     frame, *x_lims, *y_lims,
        #     util.in_range, min_pix, max_pix                
        # )
        # dist = util.dist_to_rgb_weighted(
        #     frame, avg[2], avg[1], avg[0]
        # )
        # threshold = util.threshold(dist, 20)

        # util.draw_rect(
        #     frame, *x_lims, *y_lims,
        #     util.in_range, min_pix, max_pix 
        # )

        # Show the original and processed frames
        cv2.imshow("Original", frame)
        cv2.imshow("Filter", threshold)

        # Press 'q' to exit
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release resources
    cap.release()
    cv2.destroyAllWindows()

main()
