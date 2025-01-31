cmake_minimum_required(VERSION 3.8)
project(sensors)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(OpenCV REQUIRED)

add_library(${PROJECT_NAME} SHARED src/realsense_camera_node.cpp src/basic_depth_detection.cpp src/opencv_test.cpp)
ament_target_dependencies(${PROJECT_NAME} rclcpp rclcpp_components sensor_msgs OpenCV)
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)
rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "sensors::RealsenseCameraNode"
  EXECUTABLE realsense_camera_node
)
rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "sensors::BasicDepthDetectionNode"
  EXECUTABLE depth_node
)
rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "sensors::OpenCVNode"
  EXECUTABLE cv_node
)
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
