#include <string>
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std::chrono_literals;

namespace sensors
{
class sendUDPMessage : public rclcpp::Node
{
public:
    explicit sendUDPMessage(const rclcpp::NodeOptions & options) : rclcpp::Node("udp_node", options)
    {
        
    }
}
}