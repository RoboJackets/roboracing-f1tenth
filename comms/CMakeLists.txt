cmake_minimum_required(VERSION 3.8)
project(comms)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(udp_send src/udp_client.cpp)

find_package(ament_cmake REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

target_link_libraries(udp_send ${PROJECT_NAME} ${Boost_LIBRARIES})

ament_export_dependencies(Boost)
ament_export_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

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
