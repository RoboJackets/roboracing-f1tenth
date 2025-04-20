cmake_minimum_required(VERSION 3.8)
project(wall_follower)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(ackermann_msgs REQUIRED)
find_package(std_msgs REQUIRED)


add_library(${PROJECT_NAME} SHARED src/wall_follower.cpp)
ament_target_dependencies(${PROJECT_NAME} rclcpp rclcpp_components sensor_msgs ackermann_msgs std_msgs)
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)


rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "wall_follower::WallFollower"
  EXECUTABLE wallfollower
)

ament_package()
