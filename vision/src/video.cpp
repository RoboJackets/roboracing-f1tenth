#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cstdint>
#include <queue>
#include <rclcpp_components/register_node_macro.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace vision
{
class VideoNode : public rclcpp::Node
{
public:
    explicit VideoNode(const rclcpp::NodeOptions& options) : rclcpp::Node("video", options)
    {
        // this->declare_parameter("path_to_vid", "/home/ubuntu/rj_training_container/training_ws/race.MP4");
        this->declare_parameter("path_to_vid", "/home/ubuntu/rj_training_container/training_ws/race.MP4");
        const std::string path = this->get_parameter("path_to_vid").as_string();
        this->capture = cv::VideoCapture(path);
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("~/video", 10);
        double fps = capture.get(cv::CAP_PROP_FPS);
        std::chrono::milliseconds mil((int)(1000.0 / fps));
        timer_ =  this->create_wall_timer(mil, std::bind(&VideoNode::video_callback, this));
    }
private:
    void video_callback()
    {
        if (!this->capture.isOpened())
        {
            return;
        }
        cv::Mat frame;
        this->capture >> frame;
        int count = 0;
        while (frame.empty())
        {
            if (count > 100) {
                const std::string path = this->get_parameter("path_to_vid").as_string();
                this->capture = cv::VideoCapture(path);
                count = 0;
            }
            this->capture >> frame;
            count++;
        }
        cv_bridge::CvImage cv_bridge_image;
        // auto temp = this->capture.get(cv::CAP_PROP_FOURCC)
        // cv_bridge_image.encoding = this->capture.get(cv::CAP_PROP_FOURCC);
        cv_bridge_image.encoding = "bgr8";
        cv_bridge_image.image = frame;
        publisher_->publish(*cv_bridge_image.toImageMsg());
    }

    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    cv::VideoCapture capture;
};
RCLCPP_COMPONENTS_REGISTER_NODE(vision::VideoNode)
}