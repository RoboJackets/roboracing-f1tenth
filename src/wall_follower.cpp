#include "rclcpp/rclcpp.hpp"
#include "pid_controller.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"
#include <cmath>

class WallFollower : public rclcpp::Node 
{
public:
    WallFollower() : Node("WallFollower")
    {
        drive_publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>(
            "/drive",
            rclcpp::SystemDefaultsQoS()
        );

        lidar_subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan",
            10,
            std::bind(&WallFollower::computeAngleCallback, this, std::placeholders::_1)
        );


        timer_ = this->create_wall_timer(std::chrono::milliseconds(50), std::bind(&WallFollower::DriveCallback, this));
        // PID parameters (set default values)
        double C = 1.0, P = 0.1, I = 0.01, D = 0.001, integral_bound = 10.0;
        angle_pid = PIDController(C, P, I, D, integral_bound);
    }

private:
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr drive_publisher_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr lidar_subscriber_;
    rclcpp::TimerBase::SharedPtr timer_;
    double C = 1.0, P = 0.1, I = 0.01, D = 0.001, integral_bound = 10.0;
    PIDController angle_pid = PIDController(C, P, I, D, integral_bound);
    double error = 0;
    rclcpp::Time prev_time_ = this->now();


    void computeAngleCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
        auto ranges = msg->ranges;
        double theta = 70;
        double a = ranges[(int) ((0 - msg->angle_min) / msg->angle_increment)];
        double b = ranges[(int) ((theta - msg->angle_min) / msg->angle_increment)];
        double alpha = atan((a * cos(theta) - b) / (a * sin(theta)));
        double AB = b * cos(alpha);
        double CD = AB + 1.5 * sin(alpha);

        error = 1 - CD;
    }

    void DriveCallback()
    {
        ackermann_msgs::msg::AckermannDriveStamped msg;
        msg.header.stamp = this->now();
        msg.drive.speed = 5.0;

        rclcpp::Time current_time = this->now();
        rclcpp::Duration dt = current_time - prev_time_;
        msg.drive.steering_angle = angle_pid.computePID(error, dt.seconds() / 1000);

        drive_publisher_->publish(msg);
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<WallFollower>());
    rclcpp::shutdown();
    return 0;
}
