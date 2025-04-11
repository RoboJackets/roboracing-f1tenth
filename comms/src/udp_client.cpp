#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_components/register_node_macro.hpp>
#include <std_msgs/msg/string.hpp>

using namespace std::chrono_literals;

using boost::asio::ip::udp;
namespace comms
{
class UDPClient : public rclcpp::Node
{
public:
   explicit UDPClient(const rclcpp::NodeOptions& options) : Node("udp_client", options), io_service(), socket(io_service, {udp::v4(), 8888})
   {
    io_service.run();
    subscription_ = this->create_subscription<ackermann_msgs::msg::AckermannDriveStamped>("/planning/desired_chassis_state", 10, std::bind(&UDPClient::udp_callback, this, std::placeholders::_1));
   
    do_receive();
   }
private:
   
    int count = 0;
    boost::asio::io_service io_service;
    udp::socket socket;
    udp::endpoint receiver_endpoint;
    boost::array<char, 1024> recv_buffer;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

    void udp_callback(const ackermann_msgs::msg::AckermannDriveStamped::SharedPtr msg) {

        std::string velocity = "V=" + std::to_string(msg.drive.steering_angle);
        std::string angle = "A=" + std::to_string(msg.drive.speed);
        this->do_send(velocity);
        this->do_send(angle);
        
    }

    void do_send(const std::string& message, const std::string& destination_ip, const unsigned short port) {
        std::cout << "method called!" << std::endl;
        auto remote = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(destination_ip), port);
        try {
            std::cout << "Sent!" << std::endl;
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
        std::cout << "Received: '" << std::string(recv_buffer.begin(), recv_buffer.begin()+bytes_transferred) << "'\n";

    if (!error || error == boost::asio::error::message_size)
        do_receive();
    }
    

};
    RCLCPP_COMPONENTS_REGISTER_NODE(comms::UDPClient)
}










