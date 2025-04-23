#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <sstream>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <std_msgs/msg/string.hpp>
#include "ackermann_msgs/msg/ackermann_drive_stamped.hpp"

using namespace std::chrono_literals;

using boost::asio::ip::udp;
namespace comms
{
class UDPClient : public rclcpp::Node
{
public:
   explicit UDPClient(const rclcpp::NodeOptions& options) : Node("udp_node", options), io_service(), socket(io_service, {udp::v4(), 8888})
   {
    std::cout << "starting udp" << std::endl;
    io_service.run();
    subscription_ = this->create_subscription<ackermann_msgs::msg::AckermannDriveStamped>("/comms/drive", 10, std::bind(&UDPClient::udp_callback, this, std::placeholders::_1));
    state_publisher_ = this->create_publisher<std_msgs::msg::String>("kart/state", 10);
    do_receive();
   }
private:
   
    const std::string JETSON_IP = "192.168.20.3";
    boost::asio::io_service io_service;
    udp::socket socket;
    udp::endpoint receiver_endpoint;
    boost::array<char, 1024> recv_buffer;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Subscription<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr state_publisher_;

    void udp_callback(const ackermann_msgs::msg::AckermannDriveStamped::SharedPtr msg) {
        std::string velocity = "V=" + std::to_string(msg->drive.speed);
        std::string angle = "A=" + std::to_string(msg->drive.steering_angle);
        this->do_send(velocity, JETSON_IP, 8888);
        this->do_send(angle, JETSON_IP, 8888);
        
    }

    void do_send(const std::string& message, const std::string& destination_ip, const unsigned short port) {
        auto remote = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(destination_ip), port);
        try {
            socket.send_to(boost::asio::buffer(message), remote);

        } catch (const boost::system::system_error& ex) {
            std::cout << "Not Sent!"/*things need to go here*/ << std::endl;
        }
    }

    void do_receive()
    {
        socket.async_receive_from(boost::asio::buffer(recv_buffer), receiver_endpoint,
                                boost::bind(&UDPClient::handle_receive, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));

    }
    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
    {
        std::string incoming(recv_buffer.begin(), recv_buffer.begin()+bytes_transferred);
        std::vector<std::string> split_info = splitString(incoming, ';');
        // ANGLE=%f;SPEED=%f;STATE=<STOPPED|OFF|MANUAL|AUTONOMOUS>
        if (split_info[2].compare(std::string("STOPPED")) == 0)
        {
            rclcpp::shutdown();
        }
        
        auto message = std_msgs::msg::String();
        message.data = incoming;
        state_publisher_->publish(message);

    if (!error || error == boost::asio::error::message_size)
        do_receive();
    }
    
    std::vector<std::string> splitString(const std::string& input, char delimiter)
    {
        std::istringstream stream(input);
        std::string token;
        std::vector<std::string> output;

        while (std::getline(stream, token, delimiter)) 
        {
            output.push_back(token);
        }
        return output;
    }

};
    RCLCPP_COMPONENTS_REGISTER_NODE(comms::UDPClient)
}










