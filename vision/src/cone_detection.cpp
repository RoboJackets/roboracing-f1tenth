#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cstdint>
#include <queue>
#include <rclcpp_components/register_node_macro.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

namespace vision
{
class ConeDetectionNode : public rclcpp::Node
{
public:
    explicit ConeDetectionNode(const rclcpp::NodeOptions& options) : rclcpp::Node("cone", options)
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/camera/color/image_raw", 10, std::bind(&ConeDetectionNode::color_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("~/cone", 10);
    }
private:
    void color_callback(const sensor_msgs::msg::Image & msg)
    {
        // converting sensor msg image to an OpenCV image
        auto bridge = cv_bridge::toCvCopy(msg, "bgr8");
        cv::Mat img;
        // convert to hsv
        cv::cvtColor(bridge->image, img, cv::COLOR_BGR2HSV);
        // creating a new matrix to fill with filtered values
        cv::Mat orange_scale_img(img.size(), CV_8UC1);
        // loop through rows and columns
        for (auto r = 0; r < img.rows; r++)
        {
            for (auto c = 0; c < img.cols; c++)
            {
                orange_scale_img.at<std::uint8_t>(r, c) = hsv_to_gray_orange(img.at<cv::Vec3b>(r, c));
            }
        }
        // TO DO: Test
        cv::Mat thresholded_img(orange_scale_img.size(), CV_8UC1);
        // hystereisis threshold takes into account changes to color thresholds based on lighting
        this->hysteresisThresholding(orange_scale_img, thresholded_img, 20, 40);
        this->drawBoundingBoxes(thresholded_img);
        std::cout << "publishing" << std::endl;
        // update content of bridge with updated image and corresponsing encoding
        bridge->encoding = "8UC1"; // might actually be 8UC1
        bridge->image = thresholded_img;
        // from our CVImage, we call toImageMsg() to get the underlying sensor_msgs::ImagePtr
        // Then we must dereference this pointer to access the underlying sensor_msgs::msg::Image
        publisher_->publish(*bridge->toImageMsg().get());
    }

    // convert hsv to greyscale with orange weighting
    // not orange             orange
    // black - - - - - - - - - white
    std::uint8_t hsv_to_gray_orange(cv::Vec3b hsv)
    {
        const std::uint8_t min_hue = 12;
        const std::uint8_t max_hue = 18;
        const std::uint8_t min_val = 235;
        const std::uint8_t min_sat = 235;

        const int max_dist_hue = 180 - (max_hue - min_hue);
        const int max_dist_sat = (255 - max_dist_hue) / 2;
        const int max_dist_val = (255 - max_dist_hue) / 2;

        std::uint8_t dist_hue;
        std::uint8_t dist_sat;
        std::uint8_t dist_val;
        // hue
        if (hsv[0] < min_hue)
        {
            dist_hue = min_hue - hsv[0];
        }
        else if (hsv[0] > max_hue)
        {
            if (hsv[0] < (180 + min_hue) /2)
            {
                dist_hue = hsv[0] - max_hue;
            }
            else
            {
                dist_hue = 180 + min_hue - hsv[0];
            }
        }
        else
        {
            dist_hue = 0;
        }
        // saturation
        if (hsv[1] < min_sat)
        {
            dist_sat = max_dist_sat - (hsv[1] * max_dist_sat) / (min_sat);
        }
        else
        {
            dist_sat = 0;
        }
        // value
        if (hsv[2] < min_val)
        {
            dist_val = max_dist_val - (hsv[2] * max_dist_val) / (min_val);
        }
        else
        {
            dist_val = 0;
        }
        return dist_hue + dist_sat + dist_val;
    }

    void hysteresisThresholding(cv::Mat& img, cv::Mat& out, int T_low, int T_high)
    {
        CV_Assert(out.type() == CV_8UC1); // Ensure grayscale image

        int rows = img.rows, cols = img.cols;
        out = cv::Mat::zeros(rows, cols, CV_8UC1); // Output mask

        std::queue<cv::Point> q;

        // First pass: Identify strong pixels and enqueue them
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                uchar pixel = img.at<uchar>(y, x);
                if (pixel < T_low) {
                    out.at<uchar>(y, x) = 255; // Definitely keep
                    q.push(cv::Point(x, y));  // Add to queue
                }
            }
        }

        // Directions for 8-connectivity
        int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
        int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};

        // Process queue: Propagate to medium pixels
        while (!q.empty()) {
            cv::Point p = q.front();
            q.pop();

            for (int i = 0; i < 8; ++i) {
                int nx = p.x + dx[i], ny = p.y + dy[i];

                if (nx >= 0 && nx < cols && ny >= 0 && ny < rows) {
                    uchar& neighbor = img.at<uchar>(ny, nx);
                    uchar& binPixel = out.at<uchar>(ny, nx);

                    if (neighbor >= T_low && neighbor <= T_high && binPixel == 0) {
                        binPixel = 255; // Keep the medium pixel
                        q.push(cv::Point(nx, ny)); // Continue search
                    }
                }
            }
        }
    }

    // Need to threshold for cones first using hsv/rgb filtering and hysterisis threshold
    void drawBoundingBoxes(cv::Mat& img)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(img, contours, 1, 2);
        for (int i = 0; i < contours.size(); i++)
        {
            auto cnt = contours[i];
            auto rect = cv::boundingRect(cnt);
            cv::rectangle(img, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), 128, 5);
        }
    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};
RCLCPP_COMPONENTS_REGISTER_NODE(vision::ConeDetectionNode)
}