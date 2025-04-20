#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <std_msgs/msg/string.hpp>
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"

using namespace std::chrono_literals;

using boost::asio::ip::udp;
namespace comms
{
class AckermannSend : public rclcpp::Node
{
    public: 
        explicit AckermannSend(const rclcpp::NodeOptions& options) : Node("ackermann_sender", options) {
            std::cout << "starting ackermann..." << std::endl;
            publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>("~/drive", 10);
            timer_ = this->create_wall_timer(
                std::chrono::milliseconds(500),
                std::bind(&AckermannSend::publish_drive_command, this));
            RCLCPP_INFO(this->get_logger(), "DrivePublisher node has been started");
        }
    private:
        void publish_drive_command()
        {
            auto message = ackermann_msgs::msg::AckermannDriveStamped();
            message.drive.steering_angle = 0.5;
            message.drive.speed = 2.0;
         

            publisher_->publish(message);
            RCLCPP_INFO(this->get_logger(), "Publishing: steering_angle=%.2f, speed=%.2f",
                        message.drive.steering_angle, message.drive.speed);
        }

        rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;

    };
    RCLCPP_COMPONENTS_REGISTER_NODE(comms::AckermannSend);
}