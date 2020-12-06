#include "http_connection.hpp"

http_connection::http_connection(tcp::socket socket) : socket_(std::move(socket)) {}

void http_connection::start() {
  read_request();
  check_deadline();
}

void http_connection::read_request() {
  auto self = shared_from_this();

  http::async_read(
      socket_, buffer_, request_,
      [self](beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (!ec)
          self->process_request();
      });
}

void http_connection::process_request() {
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

void http_connection::create_response() {
  if (request_.target() == "/api/status") {
    response_.set(http::field::content_type, "application/json");
    beast::ostream(response_.body()) << "{\"status\":\"OK\"}";
  } else {
    response_.result(http::status::not_found);
    response_.set(http::field::content_type, "application/json");
    beast::ostream(response_.body()) << "{\"status\":\"ERROR\"}";
  }
}

// Asynchronously transmit the response message.
void http_connection::write_response() {
  auto self = shared_from_this();

  response_.content_length(response_.body().size());

  http::async_write(socket_, response_,
                    [self](beast::error_code ec, std::size_t) {
                      self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                      self->deadline_.cancel();
                    });
}

// Check whether we have spent enough time on this connection.
void http_connection::check_deadline() {
  auto self = shared_from_this();

  deadline_.async_wait([self](beast::error_code ec) {
    if (!ec) {
      // Close socket to cancel any outstanding operation.
      self->socket_.close(ec);
    }
  });
}
