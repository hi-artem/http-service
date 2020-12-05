#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

namespace service_state {
std::size_t request_count() {
  static std::size_t count = 0;
  return ++count;
}

std::time_t now() { return std::time(0); }
}

class http_connection : public std::enable_shared_from_this<http_connection> {
public:
  http_connection(tcp::socket socket) : socket_(std::move(socket)) {}

  // Initiate the asynchronous operations associated with the connection.
  void start() {
    read_request();
    check_deadline();
  }

private:
  // The socket for the currently connected client.
  tcp::socket socket_;

  // The buffer for performing reads.
  beast::flat_buffer buffer_{8192};

  // The request message.
  http::request<http::dynamic_body> request_;

  // The response message.
  http::response<http::dynamic_body> response_;

  // The timer for putting a deadline on connection processing.
  net::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60)};

  // Asynchronously receive a complete request message.
  void read_request() {
    auto self = shared_from_this();

    http::async_read(
        socket_, buffer_, request_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
          boost::ignore_unused(bytes_transferred);
          if (!ec)
            self->process_request();
        });
  }

  // Determine what needs to be done with the request message.
  void process_request() {
    response_.version(request_.version());
    response_.keep_alive(false);

    switch (request_.method()) {
    case http::verb::get:
      BOOST_LOG_TRIVIAL(info) << "GET   200";
      response_.result(http::status::ok);
      create_response();
      break;

    case http::verb::post:
      BOOST_LOG_TRIVIAL(info) << "POST  200";
      response_.result(http::status::ok);
      create_response();
      break;

    default:
      BOOST_LOG_TRIVIAL(warning) << "ERROR 400";
      response_.result(http::status::bad_request);
      response_.set(http::field::content_type, "text/plain");
      beast::ostream(response_.body())
          << "Invalid request-method '" << std::string(request_.method_string())
          << "'";
      break;
    }

    write_response();
  }

  // Construct a response message based on the program state.
  void create_response() {
    if (request_.target() == "/api/status") {
      response_.set(http::field::content_type, "application/json");
      beast::ostream(response_.body()) << "{\"status\":\"OK\",\"request_count\":\"" << service_state::request_count() << "\",\"timestamp\":\"" << service_state::now() << "\"}";
    } else if (request_.target() == "/api") {
      response_.set(http::field::content_type, "application/json");
      beast::ostream(response_.body()) << "{\"status\":\"OK\"}";
    } else {
      response_.result(http::status::not_found);
      response_.set(http::field::content_type, "application/json");
      beast::ostream(response_.body()) << "{\"status\":\"ERROR\"}";
    }
  }

  // Asynchronously transmit the response message.
  void write_response() {
    auto self = shared_from_this();

    response_.content_length(response_.body().size());

    http::async_write(socket_, response_,
                      [self](beast::error_code ec, std::size_t) {
                        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                        self->deadline_.cancel();
                      });
  }

  // Check whether we have spent enough time on this connection.
  void check_deadline() {
    auto self = shared_from_this();

    deadline_.async_wait([self](beast::error_code ec) {
      if (!ec) {
        // Close socket to cancel any outstanding operation.
        self->socket_.close(ec);
      }
    });
  }
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor &acceptor, tcp::socket &socket) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec)
      std::make_shared<http_connection>(std::move(socket))->start();
    http_server(acceptor, socket);
  });
}

int main(int argc, char *argv[]) {
  try {
    // Check command line arguments.
    if (argc != 3) {
      BOOST_LOG_TRIVIAL(info) << "Illegal argument provided!";
      BOOST_LOG_TRIVIAL(info) << "Usage:       " << argv[0] << " <address> <port>";
      BOOST_LOG_TRIVIAL(info) << "For example: " << argv[0] << " 0.0.0.0     8080";
      return EXIT_FAILURE;
    }

    auto const address = net::ip::make_address(argv[1]);
    unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};
    http_server(acceptor, socket);

    BOOST_LOG_TRIVIAL(info)
        << "Service is listening on " << argv[1] << ":" << argv[2];
    ioc.run();
  } catch (std::exception const &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Error: " << e.what();
    return EXIT_FAILURE;
  }
}
