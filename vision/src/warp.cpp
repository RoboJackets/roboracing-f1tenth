#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class Warp
{
public:
    static void apply_top_down_projection(cv::Mat& input, cv::Mat& output) 
    {
        output = cv::Mat::zeros(input.rows, input.cols, CV_8UC1);
        cv::warpPerspective(input, output, H, input.size());
    }
private:
    static cv::Mat H;
    static const int dist = 15;
    static const int image_width = 906;
    static const int image_height = 540;
};
const std::vector<cv::Point2f> corner_pixel_coordinates = 
{
    {257, 363}, {199, 389}, {104, 432},
    {356, 368}, {321, 400}, {258, 458},
    {467, 371}, {466, 405}, {465, 471},
    {576, 371}, {611, 403}, {673, 462},
    {677, 368}, {732, 394}, {823, 439},
    {759, 362}, {827, 384}, {924, 416}
};

// !!IMPORTANT!!
// Check python vision to see how we get from this array to following one
// Understand what the transformations are doing
// Add cleaner implementation if necessary when adding camera calibration
// const std::vector<cv::Point3f> corner_grid_coordinates = {
//     {0, 0, 0}, {0, 1, 0}, {0, 2, 0},
//     {1, 0, 0}, {1, 1, 0}, {1, 2, 0},
//     {2, 0, 0}, {2, 1, 0}, {2, 2, 0},
//     {3, 0, 0}, {3, 1, 0}, {3, 2, 0},
//     {4, 0, 0}, {4, 1, 0}, {4, 2, 0},
//     {5, 0, 0}, {5, 1, 0}, {5, 2, 0}
// };

const std::vector<cv::Point3f> corner_grid_coordinates = 
{
    {305, 431, 0}, {305, 446, 0}, {305, 461, 0},
    {320, 431, 0}, {320, 446, 0}, {320, 461, 0},
    {335, 431, 0}, {335, 446, 0}, {335, 461, 0},
    {350, 431, 0}, {350, 446, 0}, {350, 461, 0},
    {365, 431, 0}, {365, 446, 0}, {365, 461, 0},
    {380, 431, 0}, {380, 446, 0}, {380, 461, 0}
};
cv::Mat Warp::H = cv::findHomography(corner_pixel_coordinates, corner_grid_coordinates); 