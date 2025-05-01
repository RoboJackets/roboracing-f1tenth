#ifndef WARP
#define WARP

#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>

class Warp
{
public:
    static void apply_top_down_projection(cv::Mat& input, cv::Mat& output) 
    {
        // if (!set_)
        // {
        set_H_inv_matrix();
                // set_ = true;
        // }
        output = cv::Mat::zeros(input.rows, input.cols, CV_8UC1);
        cv::warpPerspective(input, output, Warp::H_inv, input.size());
        // std::cout << "out = " << std::endl << " " << output << std::endl << std::endl;

    }
    static void set_H_inv_matrix()
    {
        float height = 0.3556;
        cv::Mat K = (cv::Mat_<double>(3, 3) <<
            603.47021484375,  0, 326.07391357421875,
            0,  603.1049194335938, 235.4612274169922,
            0, 0, 1
        );
        cv::Mat R = (cv::Mat_<double>(3, 3) <<
            1,  0, 0,
            0,  0, 1,
            0, -1, 0
        );
        cv::Mat t = (cv::Mat_<double>(3, 1) << 0, height, 0);
        cv::Mat n = (cv::Mat_<double>(3, 1) << 0, 1, 0);
        cv::Mat H = K * (R - (t * n.t()) / height);
        Warp::H_inv = H.inv();
        std::cout << "H_inv = " << std::endl << " " << Warp::H_inv << std::endl << std::endl;
    }
private:
    static cv::Mat H_inv;
    inline static bool set_ = false;
};
// bool Warp::set_ = false;

#endif