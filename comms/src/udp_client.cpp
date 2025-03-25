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
    subscription_ = this->create_subscription<std_msgs::msg::String>("/planning/desired_chassis_state", 10, std::bind(&UDPClient::udp_callback, this, std::placeholders::_1));
    timer_ =  this->create_wall_timer(5s, std::bind(&UDPClient::udp_timer_callback, this));
    do_receive();
   }
private:
    const std::String JETSON_IP = "192.168.20.3";
    int count = 0;
    boost::asio::io_service io_service;
    udp::socket socket;
    udp::endpoint receiver_endpoint;
    boost::array<char, 1024> recv_buffer;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

    void udp_callback(const std_msgs::msg::String & msg) {
        RCLCPP_INFO(this->get_logger(), "Received message: %s", msg.data.c_str());
        do_send(msg->data);
    }

    void udp_timer_callback() {
        if (count % 2 == 0) {
            this->do_send("V=2.0", JETSON_IP, 8888);
        } else {
            this->do_send("V=2.0", JETSON_IP, 8888);
        }
        count++;
    }

    void do_send(const std::string& message, const std::string& destination_ip, const unsigned short port) {
        try {
          remote_endpoint_ = udp::endpoint(boost::asio::ip::address::from_string(JETSON_IP), 8888);
          socket.open(remote_endpoint_.protocol());
          RCLCPP_INFO(this->get_logger(), "Sending message: %s", message.c_str(), JETSON_IP.c_str(), 8888);
        } catch (const boost::system::system::system_error& e) {
          RCLCPP_INFO(this->get_logger(), "Error: %s", e.what());
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










