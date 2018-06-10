cmake_minimum_required(VERSION 3.11) #version 3.11
project(Server)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS " -lprotobuf")
add_definitions(-DDEBUG_MODE)

set(CMAKE_AUTOMOC ON)

set(CMAKE_PREFIX_PATH "/usr/local/Cellar/qt/5.11.0/")


find_package(Boost REQUIRED COMPONENTS system thread log) #Boost libs, headers must be found.
find_package(Qt5 COMPONENTS Sql Network Core Widgets REQUIRED)

set(SRCS  Server.cpp Request_Response.pb.cc)


add_executable(Server main.cpp ${SRCS})


add_executable(placebo /Users/artur/Library/Preferences/CLion2018.1/scratches/placebo.cpp)

target_link_libraries(Server Qt5::Core Qt5::Widgets Qt5::Sql Qt5::Network)

if (Boost_FOUND)

    message(STATUS "Include dirs path:  ${Boost_INCLUDE_DIRS}") #print include path.
    message(STATUS "Libs dirs path:  ${Boost_LIBRARY_DIRS}") #print libs path.

    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(placebo ${Boost_LIBRARIES})
    target_link_libraries(Server ${Boost_LIBRARIES})
    
else()
    message(FATAL_ERROR "Boost was not found!")
endif()
