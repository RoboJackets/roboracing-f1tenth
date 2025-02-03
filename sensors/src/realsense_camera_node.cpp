#include <string>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <chrono>
#include <iostream>
#include <cstdint>
#include <vector>

using namespace std::chrono_literals;


namespace sensors
{
class RealsenseCameraNode : public rclcpp::Node
{
public:
    explicit RealsenseCameraNode(const rclcpp::NodeOptions& options)
        : rclcpp::Node("realsense_camera_node", options)
    {
        // image_msg();
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/camera/color/image_raw", 10, std::bind(&RealsenseCameraNode::topic_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("~/filter/red", 10);
        timer_ =  this->create_wall_timer(50ms, std::bind(&RealsenseCameraNode::timer_callback, this));
    }
private:
    void topic_callback(const sensor_msgs::msg::Image & msg) {

        image_msg.height = msg.height;
        image_msg.width = msg.width;
        image_msg.encoding = msg.encoding;
        image_msg.step = msg.step;
        image_msg.header = msg.header;
        image_msg.is_bigendian = msg.is_bigendian;
        length = image_msg.step * image_msg.height;
        image_msg.data = std::vector<std::uint8_t>(length);
        for (int i = 0; i < length; i++)
        {
            image_msg.data[i] = msg.data[i];
        }
    }
    void timer_callback()
    {
        for (int i = 0; i < length; i += 3) {
            if (is_orange(image_msg.data[i], image_msg.data[i + 1], image_msg.data[i + 2])) 
            {
                image_msg.data[i] = 255;
                image_msg.data[i+1] = 255;
                image_msg.data[i+2] = 255;
            }
            else
            {
                image_msg.data[i] = 0;
                image_msg.data[i+1] = 0;
                image_msg.data[i+2] = 0;
            }
        }
        publisher_->publish(image_msg);
        std::cout << "publishing" << "\n";
    }
    bool is_orange(std::uint8_t r, std::uint8_t g, std::uint8_t b)
    {
        return (r > 150 && g < 200 && b < 100);
    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    sensor_msgs::msg::Image image_msg;
    int length;
};

RCLCPP_COMPONENTS_REGISTER_NODE(sensors::RealsenseCameraNode)
}