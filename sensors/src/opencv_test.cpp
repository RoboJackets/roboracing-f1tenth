#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace sensors
{
class OpenCVNode : public rclcpp::Node
{
public:
    explicit OpenCVNode(const rclcpp::NodeOptions& options) : rclcpp::Node("cv_node", options)
    {
        color_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "camera/camera/color/image_raw",
            10,
            std::bind(&OpenCVNode::color_callback, this, std::placeholders::_1)
        );
        depth_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "camera/camera/depth/image_raw",
            10,
            std::bind(&OpenCVNode::depth_callback, this, std::placeholders::_1)
        );
    }
private:
    void color_callback(const sensor_msgs::msg::Image & msg)
    {
        // converting sensor msg image to an OpenCV image
        auto orig_img = cv_bridge::toCvShare(image_msg, "bgr8");
        cv::Mat hsv_img;
        // convert to hsv
        cv::cvtColor(orig_img, hsv_img, cv::COLOR_BGR2HSV);
        // creating a new matrix to fill with filtered values
        cv::Mat filtered(hsv_img.size(), CV_8UC1);
        // loop through rows and columns
        for (auto r = 0; r < hsv_img.rows; r++)
        {
            for (auto c = 0; c < hsv_img.columns; c++)
            {
                // convert hsv to greyscale with orange weighting
                // not orange             orange
                // black - - - - - - - - - white
                const cv::Vec3b color = hsv_img.at<cv::Vec3b>(r, c);
                output.at<uint8_t>(r, c) = 255;
            }
        }

    }

    unsigned char hsv_to_gray_orange(cv::Vec3b hsv)
    {
        const unsigned char min_hue = 10;
        const unsigned char max_hue = 20;
        const unsigned char min_val = 128;
        const unsigned char min_sat = 128;
    }

    void depth_callback(const sensor_msgs::msg::Image & msg)
    {

    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_subscription_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};
RCLCPP_COMPONENTS_REGISTER_NODE(sensors::OpenCVNode)
}