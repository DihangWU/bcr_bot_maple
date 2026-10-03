how to set up the development environment
```shell
sudo apt install ros-jazzy-ros-gz-sim ros-jazzy-ros-gz-bridge ros-jazzy-ros-gz-interfaces

# cd to a workspace directory where you want to set the project
mkdir src
cd src
git clone [URL of this project]
cd ..
rosdep install --from-paths src --ignore-src -r -y
```
how to build the project
```shell
cd [workspace dir]
colcon build --symlink-install --packages-select bcr_bot

# or
colcon build --symlink-install --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --packages-select bcr_bot
```



how to run the demo of the project
```shell
# open a new terminal in wsFolder
# launch gazebo simulation
source install/setup.bash
ros2 launch bcr_bot gz.launch.py three_d_lidar_enabled:=True

# open a new terminal in wsFolder
# launch riviz2 visuallization
source install/setup.bash
rviz2 -d ./install/bcr_bot/share/bcr_bot/rviz/sensor.view.rviz

# open a new terminal in wsFolder
# open keyboard control try 9 keys: "uiojklm,."  
source install/setup.bash
ros2 run teleop_twist_keyboard teleop_twist_keyboard --ros-args -r cmd_vel:=/bcr_bot/cmd_vel
```



how to start mapping
```shell
# open a new terminal in wsFolder
source install/setup.bash
ros2 launch bcr_bot mapping.launch.py
```

how to save the map
```shell
# open a new terminal in wsFolder
source install/setup.bash
cd src/bcr_bot/config
ros2 run nav2_map_server map_saver_cli -f diy_map_name
```

how to use the default map to navigate the robot to a specific position
```shell
# open a new terminal in wsFolder
source install/setup.bash
ros2 launch bcr_bot nav2.launch.py
```

how to use self mapped map to navigate the robot to a specific position
```shell
# open a new terminal in wsFolder
source install/setup.bash
ros2 launch bcr_bot nav2.launch.py map:=[full path to diy_map_name.yaml]
```

## Custom C++ A* global planner

The planner server is configured to load the handwritten A* implementation as
`bcr_bot::AStarPlanner`. It reads the Nav2 global costmap, avoids lethal and
inflated cells, supports diagonal motion without corner cutting, and passes
the resulting path to the configured controller.

```shell
ros2 launch bcr_bot nav2.launch.py
ros2 plugin list | grep bcr_bot::AStarPlanner
```
