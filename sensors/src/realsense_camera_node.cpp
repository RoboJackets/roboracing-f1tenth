#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>

namespace sensors
{
class RealsenseCameraNode : public rclcpp::Node
{
public:
    explicit RealsenseCameraNode(const rclcpp::NodeOptions& options)
        : rclcpp::Node("realsense_camera_node", options)
    {
        RCLCPP_INFO(get_logger(), "READY TO RUMBLE");
    }
private:
};

RCLCPP_COMPONENTS_REGISTER_NODE(sensors::RealsenseCameraNode)
}