#include "http_service.hpp"
#include "http_connection.hpp"

http_service::http_service(std::string address, std::string port) {
  this->address = net::ip::make_address(address);
  this->port = static_cast<unsigned short>(std::stoi(port));
}

void http_service::listen() {
  net::io_context ioc{1};
  tcp::acceptor acceptor{ioc, {this->address, this->port}};
  tcp::socket socket{ioc};
  this->accept_loop(acceptor, socket);
  ioc.run();
}

void http_service::accept_loop(tcp::acceptor &acceptor, tcp::socket &socket) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec)
      std::make_shared<http_connection>(std::move(socket))->start();
    accept_loop(acceptor, socket);
  });
}
