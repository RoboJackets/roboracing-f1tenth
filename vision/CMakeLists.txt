cmake_minimum_required(VERSION 3.8)
project(vision)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_library(${PROJECT_NAME} SHARED src/cone_detection.cpp src/video.cpp src/line_detection.cpp)
target_include_directories(${PROJECT_NAME}  PUBLIC ${EIGEN3_INCLUDE_DIRS})
ament_target_dependencies(${PROJECT_NAME} rclcpp rclcpp_components sensor_msgs OpenCV cv_bridge Eigen3)
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)

rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "vision::ConeDetectionNode"
  EXECUTABLE cone
)
rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "vision::LineDetectionNode"
  EXECUTABLE line
)
rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "vision::VideoNode"
  EXECUTABLE video
)
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
