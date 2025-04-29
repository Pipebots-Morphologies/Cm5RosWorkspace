cmake_minimum_required(VERSION 3.8)
project(servo_control)

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

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
ament_target_dependencies(servo_node rclcpp std_msgs)
target_link_libraries(servo_node scservo_lib)

add_executable(moveServo src/moveServo.cpp)
ament_target_dependencies(moveServo rclcpp std_msgs)
target_link_libraries(moveServo scservo_lib)

add_executable(STServoTests src/STServoTests.cpp)
ament_target_dependencies(STServoTests rclcpp std_msgs)
target_link_libraries(STServoTests scservo_lib)

add_executable(servoConfig src/servoConfig.cpp)
ament_target_dependencies(servoConfig rclcpp std_msgs)
target_link_libraries(servoConfig scservo_lib)

add_executable(servoFeedback src/servoFeedback.cpp)
ament_target_dependencies(servoFeedback rclcpp std_msgs)
target_link_libraries(servoFeedback scservo_lib)

add_executable(servoRepair src/servoRepair.cpp)
ament_target_dependencies(servoRepair rclcpp std_msgs)
target_link_libraries(servoRepair scservo_lib)

add_executable(changeId src/changeId.cpp)
ament_target_dependencies(changeId rclcpp std_msgs)
target_link_libraries(changeId scservo_lib)

# Install targets
install(TARGETS
  servo_node
  STServoTests
  moveServo
  servoConfig
  changeId
  servoFeedback
  servoRepair
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
