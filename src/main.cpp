#include "http_service.hpp"
#include <glog/logging.h>

int main(int argc, char *argv[]) {
  try {
    google::InitGoogleLogging(argv[0]);
    if (argc != 3) {
      LOG(INFO) << "Illegal argument provided!";
      LOG(INFO) << "Usage:       " << argv[0] << " <address> <port>";
      LOG(INFO) << "For example: " << argv[0] << " 0.0.0.0     8080";
      return EXIT_FAILURE;
    }

    auto service = http_service(argv[1], argv[2]);
    LOG(INFO) << "Service is listening on " << argv[1] << ":" << argv[2];
    service.listen();

  } catch (std::exception const &e) {
    LOG(FATAL) << "Error: " << e.what();
    return EXIT_FAILURE;
  }
}
