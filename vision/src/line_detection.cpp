#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cstdint>
#include <iostream>
#include <queue>
#include <rclcpp_components/register_node_macro.hpp>
#include <opencv2/core/types.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "./least_squares_line.cpp"
#include <chrono>

using namespace std::chrono_literals;

namespace vision
{
class LineDetectionNode : public rclcpp::Node
{
public:
    explicit LineDetectionNode(const rclcpp::NodeOptions& options) : rclcpp::Node("vision", options)
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>("/vision/video", 10, std::bind(&LineDetectionNode::color_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("~/line", 10);
    }
private:
    void color_callback(const sensor_msgs::msg::Image & msg)
    {
        // converting sensor msg image to an OpenCV image
        auto bridge = cv_bridge::toCvCopy(msg, "bgr8");
        cv::Mat img;
        cv::cvtColor(bridge->image, img, cv::COLOR_BGR2GRAY);
        cv::Mat blur;
        cv::GaussianBlur(img, blur, cv::Size(5, 5), 0);
        cv::Canny(blur, img, 100, 200);
        this->findLines(img, blur, 5);
        bridge->encoding = "8UC1";
        bridge->image = blur;
        publisher_->publish(*bridge->toImageMsg().get());
    }

    void findLines(cv::Mat& img, cv::Mat& out, int kernel)
    {
        int rows = img.rows, cols = img.cols;
        out = cv::Mat::zeros(rows, cols, CV_8UC1);
        findLineRight(img, out, kernel);
        findLineLeft(img, out, kernel);
    }
    void findLineLeft(cv::Mat& img, cv::Mat& out, int kernel)
    {
        int rows = img.rows, cols = img.cols;
        int horizon = 400;
        std::queue<cv::Point> queue;
        queue.push(cv::Point(rows - 1, 0));
        cv::Point p;
        bool found = false;
        do
        {
            p = queue.front();
            queue.pop();
            for (int x = 0; x < kernel; x++) 
            {
                for (int y = 0; y < kernel; y++)
                {
                    uint8_t val = img.at<uint8_t>(p.x - x, p.y + y);
                    if (val > 0 && out.at<uint8_t>(p.x - x, p.y + y) != 255) 
                    {
                        found = true;
                        // out.at<uint8_t>(p.x - x, p.y + y) = 255;
                        queue.push(cv::Point(p.x - x, p.y + y));
                    }
                }
            }
            if (!found)
            {
                queue.push(cv::Point(p.x - 1, 0));
            }
        } while (queue.size() != 0 && p.x > horizon);
    }

    void findLineRight(cv::Mat& img, cv::Mat& out, int kernel)
    {
        int rows = img.rows, cols = img.cols;
        int horizon = 400;
        std::queue<cv::Point> queue;
        queue.push(cv::Point(rows - 1, cols - 1));
        cv::Point p;
        bool found = false;
        do
        {
            p = queue.front();
            queue.pop();
            for (int x = 0; x < kernel; x++) 
            {
                for (int y = 0; y < kernel; y++)
                {
                    uint8_t val = img.at<uint8_t>(p.x - x, p.y - y);
                    if (val > 0 && out.at<uint8_t>(p.x - x, p.y - y) != 255) 
                    {
                        found = true;
                        out.at<uint8_t>(p.x - x, p.y - y) = 255;
                        queue.push(cv::Point(p.x - x, p.y - y));
                    }
                }
            }
            if (!found)
            {
                queue.push(cv::Point(p.x - 1, cols - 1));
            }
        } while (queue.size() != 0 && p.x > horizon);
    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};
RCLCPP_COMPONENTS_REGISTER_NODE(vision::LineDetectionNode)
}