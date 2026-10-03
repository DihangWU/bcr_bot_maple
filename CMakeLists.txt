cmake_minimum_required(VERSION 3.5)
project(bcr_bot)

find_package(ament_cmake REQUIRED)
include(algorithm/astar/CMakeLists.txt)

install(DIRECTORY launch robot simulation rviz config
    DESTINATION share/${PROJECT_NAME}/
)

ament_package()