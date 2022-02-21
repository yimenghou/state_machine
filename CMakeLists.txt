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
find_package(fmt REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(
  include
)

set(dependencies
  behaviortree_cpp_v3
)

###############################################

add_library(${PROJECT_NAME} SHARED
  src/sm.cpp
  src/state.cpp
  src/event.cpp
)
ament_target_dependencies(${PROJECT_NAME}
  ${dependencies}
)

# add_executable(dummy_sm test/dummy_sm.cpp)
# ament_target_dependencies(dummy_sm
#   ${dependencies}
# )
# target_link_libraries(dummy_sm
#   ${PROJECT_NAME}
# )

install(TARGETS ${PROJECT_NAME}
  # EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# install(TARGETS dummy_sm
#   DESTINATION lib/${PROJECT_NAME}
# )

install(DIRECTORY include/
  DESTINATION include/
)

# install(DIRECTORY
  # launch
  # config
  # DESTINATION share/${PROJECT_NAME}/
# )

if(BUILD_TESTING)

  find_package(ament_cmake_gtest REQUIRED)
  # find_package(ament_cmake_pytest REQUIRED)

  ament_add_gtest(test_state_machine 
    test/test_state_machine.cpp
  )
  target_link_libraries(test_state_machine
    ${PROJECT_NAME}
  )
  ament_target_dependencies(test_state_machine
    ${dependencies}
  )

endif()

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
# ament_export_targets(${PROJECT_NAME})
ament_export_dependencies(${dependencies})
ament_package()