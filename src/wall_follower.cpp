#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"
#include <algorithm>
#include <cmath>

class WallFollower : public rclcpp::Node 
{
public:
    WallFollower() : Node("wall_follower")
    {
        this->declare_parameter<double>("C",1);
        this->declare_parameter<double>("P", 1);
        this->declare_parameter<double>("I", 0);
        this->declare_parameter<double>("D", 0.001);
        this->declare_parameter<double>("integral_bound", 5);
        this->declare_parameter<double>("velocity", 5);
        this->declare_parameter<double>("desired_trajectory", 1);

        C = this->get_parameter("C").as_double();
        P = this->get_parameter("P").as_double();
        I = this->get_parameter("I").as_double();
        D = this->get_parameter("D").as_double();
        velocity = this->get_parameter("velocity").as_double();
        integral_bound = this->get_parameter("integral_bound").as_double();
        desired_trajectory = this->get_parameter("desired_trajectory").as_double();

        RCLCPP_INFO_ONCE(this->get_logger(), "C: %f\tP: %f\t I: %f\tD: %f\t Integral Bound: %f", C, P, I, D, integral_bound);


        drive_publisher_ = this->create_publisher<ackermann_msgs::msg::AckermannDriveStamped>(
            "/drive",
            rclcpp::SystemDefaultsQoS()
        );

        lidar_subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan",
            10,
            std::bind(&WallFollower::angleCallback, this, std::placeholders::_1)
        );


        timer_ = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&WallFollower::DriveCallback, this));
    }

private:
    rclcpp::Publisher<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr drive_publisher_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr lidar_subscriber_;
    rclcpp::TimerBase::SharedPtr timer_;
    double C;
    double P;
    double I;
    double D;
    double integral_bound;
    double velocity;
    double desired_trajectory;
    double integral_error = 0.0;
    double prev_error = 0.0;
    double error = 0;

    rclcpp::Time prev_time_ = this->now();


    void angleCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    //     RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, 
    // "LiDAR Info -> angle_min: %f, angle_max: %f, angle_increment: %f, range_size: %lu",
    // msg->angle_min, msg->angle_max, msg->angle_increment, msg->ranges.size());


        double theta = 70 * M_PI / 180;
        int a_index = (int) (((theta/1.5) - msg->angle_min) / msg->angle_increment);
        int b_index = (int) (((-theta/1.5) - msg->angle_min) / msg->angle_increment);
        
        //RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, 
    //"Calculated Indices -> a_index: %d, b_index: %d", a_index, b_index);


        float a = msg->ranges[a_index];
        float b = msg->ranges[b_index];

        if (!std::isfinite(a) || !std::isfinite(b)) {
            RCLCPP_WARN(this->get_logger(), "Invalid LiDAR readings detected, skipping frame.");
            return;
        }
                
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Distance A: %f\t Distance B: %f", a, b);
        error = a - 1.5;
        // double alpha = atan((a * cos(theta) - b) / (a * sin(theta)));
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Error: %f", error);
        // double AB = b * cos(alpha);
        // double CD = AB + 1.5 * sin(alpha);

        // double middle_distance = (a + b) / 2.0;
        // error = desired_trajectory - middle_distance;
    }

    void DriveCallback()
    {
        ackermann_msgs::msg::AckermannDriveStamped msg;
        msg.header.stamp = this->now();
        msg.drive.speed = velocity;

        rclcpp::Time current_time = this->now();
        rclcpp::Duration dt = current_time - prev_time_;
        
        msg.drive.steering_angle = computePID(dt.seconds() / 1000);

        drive_publisher_->publish(msg);
    }

    double computePID(double dt) {
        integral_error = std::min(integral_bound, std::max(integral_error + error, -integral_bound));
        double error_delta = (error - prev_error) / dt;
        prev_error = error;
        return P * error + integral_error * I + error_delta * D;
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<WallFollower>());
    rclcpp::shutdown();
    return 0;
}
