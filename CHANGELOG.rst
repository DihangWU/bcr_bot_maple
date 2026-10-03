^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package bcr_bot
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.1.1.2 (2026-10-03)
--------------------
    * bug fix

2.1.1.1 (2026-10-03)
--------------------
    * optimize the project directory structure

2.1.1 (2026-10-03)
--------------------
    1. add .clangd configuration file
    2. adjuest the .gitignore
    3. update the CHANGELOG.rst file, remove DevLog.md file
    4. remove the Docker related files
    5. decouple the A* planning algorithm from main project step 2
        * CMakeLists.txt decoupling


2.1.0.1 (2026-10-03)
--------------------
    * Decouple A* planning algorithm from main project step 1

2.1.0 (2026-10-03)
------------------
    * Handwrite A* global path planner replacing System's Nav2 Planner
    * A* planner with cost-aware heuristic and corner-cutting prevention

2.0.1.3 (2026-10-01)
--------------------
    * 3D lidar detection parameter update

2.0.1.2 (2026-09-30)
--------------------
    * add README.md guidance

2.0.1.1 (2026-09-30)
--------------------
    * optimize Maple.rviz to better display

2.0.1 (2026-09-30)
------------------
    * change the 3D lidar's horizontal angle range to -60 to 60, keep the vertical angle range to -15 to 15
    * update the config file "Maple.rviz" to better display

2.0.0 (2026-09-29)
------------------
    1. add new README.md file, change the origin README.md to README(bc).md
        * add "how to build the project"
        * add "how to run the demo of the project"
    2. add new DevLog.md file
    3. add 3D lidar funciton
    4. add new rviz config file "Maple.rviz" to configure the rviz2 display
    5. change kinect camera's effective distance to 20m
    6. remove unused function "get_xacro_to_doc"


------------------------------------------------------------------------------
------------------------------------------------------------------------------
Above is Maple Version
------------------------------------------------------------------------------
------------------------------------------------------------------------------


1.0.0
---------------------------------------
* Update MuJoCo simulation image in README
* Update package maintainer metadata
* Enable GPU passthrough in Docker Compose configuration
* Refactor Dockerfile: consolidate package installations and upgrade base ROS image
* Add Docker Compose support and MuJoCo floor texture
* Add MuJoCo small warehouse world and corresponding navigation map
* Integrate MuJoCo control and sensor plugins
* Add MuJoCo robot model with mujoco_ros2_control integration

1.0.0 (2025-11-26)
------------------
* Update package for ROS 2 Jazzy support
* Update Nav2 configuration parameters to run Navigation2 on Jazzy
* Remove test dependencies from the package

0.0.3 (2024-11-12)
------------------
* Update USD model files for Isaac Sim compatibility
* Add USD support for NVIDIA Isaac Sim

0.0.2.1 (2024-07-02)
--------------------
* Add SLAM mapping and Nav2 navigation support
* Update package README

0.0.2 (2024-06-12)
------------------
* Add support for Gazebo Harmonic (Gazebo Sim)
* Add base_footprint TF frame
* Set Gazebo resource paths through launch files
* Modify launch files to dynamically spawn bcr_bot