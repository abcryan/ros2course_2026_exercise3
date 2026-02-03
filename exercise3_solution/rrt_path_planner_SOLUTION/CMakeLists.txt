cmake_minimum_required(VERSION 3.8)
project(rrt_path_planner)

# Ensure that we are using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)

# Find custom action definitions
find_package(rosidl_default_generators REQUIRED)

# Generate action messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/PlanPath.action"
  DEPENDENCIES std_msgs geometry_msgs nav_msgs
)

# Include directories
include_directories(include)

# Add source files
add_executable(planning_action_server src/planning_action_server.cpp)
add_executable(planning_action_client src/planning_action_client.cpp)
add_executable(planning_action_visualizer src/planning_action_visualizer.cpp)

# Link dependencies
ament_target_dependencies(planning_action_server 
  rclcpp 
  rclcpp_action 
  std_msgs 
  geometry_msgs 
  nav_msgs
)

ament_target_dependencies(planning_action_client 
  rclcpp 
  rclcpp_action 
  std_msgs 
  geometry_msgs 
  nav_msgs
)

ament_target_dependencies(planning_action_visualizer 
  rclcpp 
  rclcpp_action 
  std_msgs 
  geometry_msgs 
  nav_msgs
)

# Link the generated action files
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")

target_link_libraries(planning_action_server ${cpp_typesupport_target})
target_link_libraries(planning_action_client ${cpp_typesupport_target})
target_link_libraries(planning_action_visualizer ${cpp_typesupport_target})

# Install executables
install(TARGETS
  planning_action_server
  planning_action_client
  planning_action_visualizer
  DESTINATION lib/${PROJECT_NAME}
)

# Install headers
install(DIRECTORY include/
  DESTINATION include/
)

# Install launch files
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/
)

# Install rviz files
install(DIRECTORY rviz/
  DESTINATION share/${PROJECT_NAME}/rviz
)

# Install config files
install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)


# Export package information
ament_export_include_directories(include)
ament_export_dependencies(rclcpp rclcpp_action std_msgs geometry_msgs nav_msgs)

# Generate the action message and make sure it's available
ament_package()
