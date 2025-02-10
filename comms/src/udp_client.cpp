#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <iostream>

using boost::asio::ip::udp;

class UDPClient
{
public:
    boost::asio::io_service io_service;
    udp::socket socket;
    udp::endpoint receiver_endpoint;
    boost::array<char, 1024> recv_buffer;

    UDPClient();
    void do_receive();
    void handle_receive(const boost::system::error_code& error, size_t);
    void do_send(const std::string& message, const std::string& destination_ip, const unsigned short port);
};

UDPClient::UDPClient()
    : io_service(),
      socket(io_service, {udp::v4(), 8888})
{
    do_receive();
    io_service.run();
}

void UDPClient::do_receive()
{
    socket.async_receive_from(boost::asio::buffer(recv_buffer), receiver_endpoint,
                               boost::bind(&UDPClient::handle_receive, this,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));
}

void UDPClient::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
    std::cout << "ulala" << std::endl;
    std::cout << "Received: '" << std::string(recv_buffer.begin(), recv_buffer.begin()+bytes_transferred) << "'\n";

    if (!error || error == boost::asio::error::message_size)
        do_receive();
}

void UDPClient::do_send(const std::string& message, const std::string& destination_ip, const unsigned short port) {
	auto remote = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(destination_ip), port);
	try {
		socket.send_to(boost::asio::buffer(message), remote);

	} catch (const boost::system::system_error& ex) {
		std::cout << "Not Sent!" << std::endl;
	}
}