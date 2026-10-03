cmake_minimum_required(VERSION 3.5)
project(bcr_bot)

find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav2_core REQUIRED)
find_package(nav2_costmap_2d REQUIRED)
find_package(nav2_util REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2_ros REQUIRED)

add_library(bcr_bot_astar_core algorithm/astar/src/astar.cpp)
target_compile_features(bcr_bot_astar_core PUBLIC cxx_std_17)
target_include_directories(bcr_bot_astar_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/algorithm/astar/include> $<INSTALL_INTERFACE:include>)
add_library(bcr_bot_astar_planner SHARED algorithm/astar/src/astar_planner.cpp)
target_compile_features(bcr_bot_astar_planner PUBLIC cxx_std_17)
target_include_directories(bcr_bot_astar_planner PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/algorithm/astar/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(bcr_bot_astar_planner bcr_bot_astar_core)
ament_target_dependencies(bcr_bot_astar_planner geometry_msgs nav2_core nav2_costmap_2d nav2_util nav_msgs pluginlib rclcpp tf2_ros)
pluginlib_export_plugin_description_file(nav2_core algorithm/astar/astar_planner_plugin.xml)
install(TARGETS bcr_bot_astar_core bcr_bot_astar_planner ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY algorithm/astar/include/ DESTINATION include)

install(DIRECTORY launch meshes models urdf worlds rviz config mjcf
    DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_astar algorithm/astar/test/test_astar.cpp)
  target_link_libraries(test_astar bcr_bot_astar_core)
  target_include_directories(test_astar PRIVATE algorithm/astar/include)
endif()
         
ament_package()