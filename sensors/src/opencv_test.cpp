#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <rclcpp_components/register_node_macro.hpp>
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
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_subscription_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
}
RCLCPP_COMPONENTS_REGISTER_NODE(sensors::OpenCVNode)
}