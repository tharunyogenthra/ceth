cmake_minimum_required(VERSION 3.10)
project(CethApp)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/websocketpp)

add_executable(app src/main.cpp)

target_link_libraries(app ssl crypto pthread)
