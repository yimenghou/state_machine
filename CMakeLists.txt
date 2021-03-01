cmake_minimum_required(VERSION 3.5)
project(state_machine)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(include)

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

install(TARGETS ${PROJECT_NAME}
  # EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS test_state_machine
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include/
)

install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(${state_library} ${event_library} ${PROJECT_NAME})
# ament_export_targets(${PROJECT_NAME})
ament_export_dependencies(${dependencies})
ament_package()