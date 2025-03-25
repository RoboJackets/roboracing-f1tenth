#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>


using namespace std::chrono_literals;

using namespace comms
{
  class MotorControl : public rclcpp::Node{
    public:
      explicit MotorControl(const rclcpp::NodeOptions& options) : NodeOptions("MotorControl", options)
      {
        publisher_ = this->create_publisher(std_msgs::msg::String>("/planning/desired_chassis_state", 10);
        timer_ = this->create_wall_timer(std::chrono::seconds(2), std::bind(&MotorControl::publish_velocity, this));
      }
      private:
        void publish_velocity() {
          auto message = std_msgs::msg::String();
          message.data = "V=1.5";
          RCLCPP_INFO(this->get_logger(), "publish_velocity: %s", message.data.c_str());
          publisher_.publish(message);
        }

        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  };

  int main (int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorControl>());
    rclcpp::shutdown();
    return 0;
  }



