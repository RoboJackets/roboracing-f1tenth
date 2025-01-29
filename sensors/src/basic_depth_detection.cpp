#include<string>
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
class BasicDepthDetectionNode : public rclcpp::Node
{
public:
    explicit BasicDepthDetectionNode(const rclcpp::NodeOptions& options)
        : rclcpp::Node("realsense_camera_node", options)
    {
        // image_msg();
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/camera/depth/image_rect_raw", 10, std::bind(&BasicDepthDetectionNode::topic_callback, this, std::placeholders::_1));
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("~/filter/depth", 10);
        timer_ =  this->create_wall_timer(50ms, std::bind(&BasicDepthDetectionNode::timer_callback, this));
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
        for (int i = 0; i < length; i +=2 ) 
        {
            unsigned short depth = (((unsigned short) image_msg.data[i]) << 8) + image_msg.data[i+1];
            if (depth < 300) 
            {
                depth = 0;
            }
            else
            {
                depth = 65000;
            }
            unsigned char left = (depth >> 8);
            unsigned char right = (depth & 0xFF);
            image_msg.data[i] = left;
            image_msg.data[i+1] = right;
        }
        publisher_->publish(image_msg);
        std::cout << "publishing" << "\n";
    }
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    sensor_msgs::msg::Image image_msg;
    int length;
};

RCLCPP_COMPONENTS_REGISTER_NODE(sensors::BasicDepthDetectionNode)
}