#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include "http_connection.hpp"
#include <boost/asio.hpp>

namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

class http_service {
public:
  http_service(std::string address, std::string port);
  void listen();

private:
  net::ip::address address;
  unsigned short port;

  void accept_loop(tcp::acceptor &acceptor, tcp::socket &socket);
};

#endif
