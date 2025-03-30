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
    static const int image_width = 1920;
    static const int image_height = 1080;
};
const std::vector<cv::Point2f> corner_pixel_coordinates = 
{
    {257*2, 363*2}, {199*2, 389*2}, {104*2, 432*2},
    {356*2, 368*2}, {321*2, 400*2}, {258*2, 458*2},
    {467*2, 371*2}, {466*2, 405*2}, {465*2, 471*2},
    {576*2, 371*2}, {611*2, 403*2}, {673*2, 462*2},
    {677*2, 368*2}, {732*2, 394*2}, {823*2, 439*2},
    {759*2, 362*2}, {827*2, 384*2}, {924*2, 416*2}
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
    {930, 989, 0}, {930, 1019, 0}, {930, 1049, 0},
    {960, 989, 0}, {960, 1019, 0}, {960, 1049, 0},
    {990, 989, 0}, {990, 1019, 0}, {990, 1049, 0},
    {1020, 989, 0}, {1020, 1019, 0}, {1020, 1049, 0},
    {1050, 989, 0}, {1050, 1019, 0}, {1050, 1049, 0},
    {1080, 989, 0}, {1080, 1019, 0}, {1080, 1049, 0}
};
cv::Mat Warp::H = cv::findHomography(corner_pixel_coordinates, corner_grid_coordinates); 