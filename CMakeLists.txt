cmake_minimum_required(VERSION 2.8.3)
project(state_machine)

set (CMAKE_CXX_STANDARD 17)

find_package(catkin REQUIRED)

find_package(Boost REQUIRED COMPONENTS system)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  # CATKIN_DEPENDS
#  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

############## events library ##############

add_library(dummy_events
  src/event/dummy_events.cpp
)

list(APPEND event_library dummy_events)

###############################################

############## states library ##############

add_library(dummy_states
  src/state/dummy_states.cpp
)
target_link_libraries(dummy_states ${event_library})


list(APPEND state_library dummy_states)

###############################################

add_library(${PROJECT_NAME} src/sm.cpp)
target_link_libraries(${PROJECT_NAME} ${state_library} ${event_library})

add_executable(test_state_machine test/test_sm.cpp)
target_link_libraries(test_state_machine ${PROJECT_NAME})
