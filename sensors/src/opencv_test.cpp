#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cstdint>
#include <rclcpp_components/register_node_macro.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
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
    void color_callback(const sensor_msgs::msg::Image::ConstSharedPtr & msg)
    {
        // converting sensor msg image to an OpenCV image
        auto orig_img = cv_bridge::toCvCopy(msg, "bgr8");
        // convert to hsv
        cv::cvtColor(orig_img->image, orig_img->image, cv::COLOR_BGR2HSV);
        // creating a new matrix to fill with filtered values
        cv::Mat orange_scale_img(orig_img->image.size(), CV_8UC1);
        // loop through rows and columns
        for (auto r = 0; r < orig_img->image.rows; r++)
        {
            for (auto c = 0; c < orig_img->image.cols; c++)
            {
                orig_img->image.at<std::uint8_t>(r, c) = hsv_to_gray_orange(orig_img->image.at<cv::Vec3b>(r, c));
            }
        }
        cv::Mat thresholded_img(orig_img->image.size(), CV_8UC1);
        // adaptive threshold takes into account changes to color thresholds based on lighting
        cv::adaptiveThreshold(orange_scale_img, thresholded_img, 255,
            cv::THRESH_BINARY, cv::ADAPTIVE_THRESH_MEAN_C, 11, 2);
        // from our CVImage, we call toImageMsg() to get the underlying sensor_msgs::ImagePtr
        // Then we must dereference this pointer to access the underlying sensor_msgs::msg::Image
        publisher_->publish(*orig_img->toImageMsg().get());
    }

    // convert hsv to greyscale with orange weighting
    // not orange             orange
    // black - - - - - - - - - white
    std::uint8_t hsv_to_gray_orange(cv::Vec3b hsv)
    {
        const std::uint8_t min_hue = 10;
        const std::uint8_t max_hue = 20;
        const std::uint8_t min_val = 128;
        const std::uint8_t min_sat = 128;

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
            dist_sat = max_dist_sat - (hsv[1] * max_dist_sat) / (255 - min_sat);
        }
        else
        {
            dist_sat = 0;
        }
        // value
        if (hsv[2] < min_val)
        {
            dist_val = max_dist_val - (hsv[2] * max_dist_val) / (255 - min_val);
        }
        else
        {
            dist_val = 0;
        }
        return dist_hue + dist_sat + dist_val;
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