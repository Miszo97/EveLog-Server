cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS " -lprotobuf")
add_definitions(-DDEBUG_MODE)

set(CMAKE_AUTOMOC ON)

set(CMAKE_PREFIX_PATH "/usr/local/Cellar/qt/5.11.0/")

include_directories(/usr/local/boost_1_67_0/)

find_package(Qt5Widgets  REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)



set(SRCS  Server.cpp Request_Response.pb.cc)


add_executable(Server main.cpp ${SRCS})


target_link_libraries(Server Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network)

