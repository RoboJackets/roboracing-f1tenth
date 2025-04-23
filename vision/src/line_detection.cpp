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
#include <chrono>
#include <cmath>
// #include "./spline_function.cpp"

using namespace std::chrono_literals;

namespace vision
{
class LineDetectionNode : public rclcpp::Node
{
public:
    explicit LineDetectionNode(const rclcpp::NodeOptions& options) : rclcpp::Node("line", options)
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
        // cv::cvtColor(bridge->image, img, cv::COLOR_BGR2GRAY);
        cv::Mat img2;
        // this->cropHorizon(bridge->image, img2, .48);
        // this->filterRoad(img2, img);
        // cv::GaussianBlur(img, img2, cv::Size(5, 5), 0);
        // cv::Canny(img, img2, 100, 200);
        this->dist_to_white(img, img2);
        cv::threshold(img2, img, 70, 255, cv::THRESH_BINARY_INV);
        // std::vector<int> x_left, y_left, x_right, y_right;
        // this->findLines(img, img2, 5);
        bridge->encoding = "8UC1";
        // SplineFunction s(xvals, yvals);
        // std::cout << s(12.34) << std::endl;
        bridge->image = img;
        publisher_->publish(*bridge->toImageMsg().get());
    }

    void cropHorizon(cv::Mat& img, cv::Mat& out, float percent_start)
    {
        int rows = img.rows, cols = img.cols;
        cv::Rect crop(0, rows*percent_start, cols, rows-rows*percent_start);
        out = img(crop);
    }

    void findLines(cv::Mat& img, cv::Mat& out, int kernel)
    {
        int rows = img.rows, cols = img.cols;
        out = cv::Mat::zeros(rows, cols, CV_8UC1);
        // findLineRight(img, out, kernel);
        // findLineLeft(img, out, kernel);
    }

    void findLineLeft(cv::Mat& img, cv::Mat& out, int kernel, std::vector<int> x_list, std::vector<int> y_list)
    {
        int rows = img.rows, cols = img.cols;
        int horizon = 400;
        std::queue<cv::Point> queue;
        queue.push(cv::Point(rows - 1, 0));
        cv::Point p;
        bool found = false;
        int count = 0;
        do
        {
            p = queue.front();
            queue.pop();
            if (count % 25 == 0 && found)
            {
                x_list.push_back(p.x);
                y_list.push_back(p.y);
            }
            for (int x = 0; x < kernel; x++) 
            {
                for (int y = 0; y < kernel; y++)
                {
                    uint8_t val = img.at<uint8_t>(p.x - x, p.y + y);
                    if (val > 0 && out.at<uint8_t>(p.x - x, p.y + y) != 255) 
                    {
                        found = true;
                        out.at<uint8_t>(p.x - x, p.y + y) = 255;
                        queue.push(cv::Point(p.x - x, p.y + y));
                    }
                }
            }
            if (!found)
            {
                queue.push(cv::Point(p.x - 1, 0));
            }
            count += 1;
        } while (queue.size() != 0 && p.x > horizon);
    }

    void dist_to_white(const cv::Mat& img, cv::Mat& out)
    {
        out = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
        cv::Mat dr(img.rows, img.cols, CV_8UC1);
        for (int r = 0; r < img.rows; r++)
        {
            for (int c = 0; c < img.cols; c++) {
                cv::Vec3b pix = img.at<cv::Vec3b>(r, c);
                int dr = 255 - pix[2]; 
                int dg = 255 - pix[1]; 
                int db = 255 - pix[0];
                int dist_squared = dr * dr + dg * dg + db * db;
                int dist = std::sqrt((double)dist_squared);
                if (dist > 255) {
                    dist = 255;
                }
                out.at<uint8_t>(r, c) = dist;
            }
        }
    }

    void findLineRight(const cv::Mat& img, cv::Mat& out, int kernel, std::vector<int> x_list, std::vector<int> y_list)
    {
        int rows = img.rows, cols = img.cols;
        int horizon = 400;
        std::queue<cv::Point> queue;
        queue.push(cv::Point(rows - 1, cols - 1));
        cv::Point p;
        bool found = false;
        int count = 0;
        do
        {
            p = queue.front();
            queue.pop();
            if (count % 25 == 0 && found)
            {
                x_list.push_back(p.x);
                y_list.push_back(p.y);
            }
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
            count += 1;
        } while (queue.size() != 0 && p.x > horizon);
    }

    void filterRoad(const cv::Mat& img, cv::Mat& out)
    {
        out = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
        int rows = img.rows, cols = img.cols;
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                cv::Vec3b bgr = img.at<cv::Vec3b>(r, c);
                int avg = (((int)bgr[0] + (int)bgr[1] + (int)bgr[2])/3);
                int dist_squared = (avg - bgr[0])*(avg - bgr[0]) + (avg - bgr[1])*(avg - bgr[1]) + (avg - bgr[2])*(avg - bgr[2]);
                uint8_t dist = sqrt(dist_squared);
                out.at<uint8_t>(r, c) = dist;
            }
        }
    }
    
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};
RCLCPP_COMPONENTS_REGISTER_NODE(vision::LineDetectionNode)
}