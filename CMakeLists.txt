cmake_minimum_required(VERSION 2.8.3)
project(state_machine)

set (CMAKE_CXX_STANDARD 17)

find_package(catkin REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

catkin_package()

include_directories(
  include
)

## Declare a C++ library
add_executable(${PROJECT_NAME}_exe
  src/main.cpp
)