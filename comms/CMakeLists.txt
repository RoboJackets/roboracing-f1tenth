cmake_minimum_required(VERSION 3.8)
project(comms)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(ackermann_msgs REQUIRED)


add_library(${PROJECT_NAME} SHARED src/udp_client.cpp src/ackermann_send.cpp)
ament_target_dependencies(${PROJECT_NAME} rclcpp rclcpp_components std_msgs ackermann_msgs)
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)


rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "comms::UDPClient"
  EXECUTABLE udp
)

rclcpp_components_register_node(
  ${PROJECT_NAME}
  PLUGIN "comms::AckermannSend"
  EXECUTABLE ackermann_send
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
