#include <boost/log/trivial.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "http_service.hpp"

int main(int argc, char *argv[]) {
  try {
    if (argc != 3) {
      BOOST_LOG_TRIVIAL(info) << "Illegal argument provided!";
      BOOST_LOG_TRIVIAL(info) << "Usage:       " << argv[0] << " <address> <port>";
      BOOST_LOG_TRIVIAL(info) << "For example: " << argv[0] << " 0.0.0.0     8080";
      return EXIT_FAILURE;
    }

    auto service = http_service(argv[1], argv[2]);
    BOOST_LOG_TRIVIAL(info) << "Service is listening on " << argv[1] << ":" << argv[2];
    service.listen();

  } catch (std::exception const &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Error: " << e.what();
    return EXIT_FAILURE;
  }
}
