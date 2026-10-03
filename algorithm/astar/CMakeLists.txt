# Include this file from the package CMakeLists.txt. Sharing its directory scope
# lets ament_package() consume pluginlib's registration data and package hook.
# Resolve source paths relative to this file, not the including directory.
set(BCR_ASTAR_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")

find_package(geometry_msgs REQUIRED)
find_package(nav2_core REQUIRED)
find_package(nav2_costmap_2d REQUIRED)
find_package(nav2_util REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2_ros REQUIRED)

add_library(bcr_bot_astar_core "${BCR_ASTAR_SOURCE_DIR}/src/astar.cpp")
target_compile_features(bcr_bot_astar_core PUBLIC cxx_std_17)
target_include_directories(bcr_bot_astar_core PUBLIC
  $<BUILD_INTERFACE:${BCR_ASTAR_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

add_library(bcr_bot_astar_planner SHARED "${BCR_ASTAR_SOURCE_DIR}/src/astar_planner.cpp")
target_compile_features(bcr_bot_astar_planner PUBLIC cxx_std_17)
target_include_directories(bcr_bot_astar_planner PUBLIC
  $<BUILD_INTERFACE:${BCR_ASTAR_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(bcr_bot_astar_planner bcr_bot_astar_core)
ament_target_dependencies(bcr_bot_astar_planner
  geometry_msgs nav2_core nav2_costmap_2d nav2_util nav_msgs pluginlib rclcpp tf2_ros)

# pluginlib expects a path relative to CMAKE_CURRENT_SOURCE_DIR, which remains
# the package directory when this file is included.
file(RELATIVE_PATH BCR_ASTAR_PLUGIN_XML "${CMAKE_CURRENT_SOURCE_DIR}"
  "${BCR_ASTAR_SOURCE_DIR}/astar_planner_plugin.xml")
pluginlib_export_plugin_description_file(nav2_core "${BCR_ASTAR_PLUGIN_XML}")

install(TARGETS bcr_bot_astar_core bcr_bot_astar_planner
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(DIRECTORY "${BCR_ASTAR_SOURCE_DIR}/include/" DESTINATION include)

# Test sources may be omitted from this workspace; enable only those present.
if(BUILD_TESTING AND EXISTS "${BCR_ASTAR_SOURCE_DIR}/test/test_astar.cpp")
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_astar "${BCR_ASTAR_SOURCE_DIR}/test/test_astar.cpp")
  target_link_libraries(test_astar bcr_bot_astar_core)
endif()
if(BUILD_TESTING AND EXISTS "${BCR_ASTAR_SOURCE_DIR}/tests/test_plugin_loading.cpp")
  find_package(ament_cmake_gtest REQUIRED)
  # Load via the ament index, just as planner_server does; do not link our plugin.
  ament_add_gtest(test_astar_plugin_loading
    "${BCR_ASTAR_SOURCE_DIR}/tests/test_plugin_loading.cpp")
  ament_target_dependencies(test_astar_plugin_loading nav2_core pluginlib)
  add_dependencies(test_astar_plugin_loading bcr_bot_astar_planner)
endif()
