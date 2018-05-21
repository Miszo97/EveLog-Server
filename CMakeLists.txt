cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")
set (CMAKE_CXX_FLAGS "-Ddebug_cerr -lprotobuf")

set(CMAKE_AUTOMOC ON)



find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)



add_executable(Server main.cpp Server.cpp Server.hpp)

target_link_libraries(Server  Qt5::Widgets Qt5::Core Qt5::Sql Qt5::Network)
