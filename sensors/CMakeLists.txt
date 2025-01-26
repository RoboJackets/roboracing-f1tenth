cmake_minimum_required(VERSION 3.8)
project(sensors)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)

add_library(${PROJECT_NAME} SHARED src/realsense_camera_node.cpp)
ament_target_dependencies(${PROJECT_NAME} rclcpp rclcpp_components)
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)
rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "sensors::RealsenseCameraNode"
  EXECUTABLE realsense_camera_node
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
