cmake_minimum_required(VERSION 3.10)

project(http-service VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(http-service src/main.cpp)
target_link_libraries(http-service ${CONAN_LIBS})
