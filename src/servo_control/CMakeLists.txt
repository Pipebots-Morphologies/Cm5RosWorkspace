cmake_minimum_required(VERSION 3.8)
project(servo_control)

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Include directories
include_directories(
  include
  include/servo_control
)

# Automatically include all source files in scservo/src
file(GLOB_RECURSE SCSERVO_SOURCES
  "src/lib/*.cpp"
)

# Create the scservo library
add_library(scservo_lib ${SCSERVO_SOURCES})

# Add your ROS 2 node executable
add_executable(servo_node src/servo_node.cpp)
ament_target_dependencies(servo_node rclcpp)
target_link_libraries(servo_node scservo_lib)

# Install targets
install(TARGETS
  servo_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
