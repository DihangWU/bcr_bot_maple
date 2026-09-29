how to build the project
```shell
cd bcr_maple_ws
colcon build --symlink-install --packages-select bcr_bot
```



how to run the demo of the project
```shell
# open a new terminal in wsFolder
source install/setup.bash
ros2 launch bcr_bot gz.launch.py three_d_lidar_enabled:=True

# open a new terminal in wsFolder
source install/setup.bash
rviz2 -d ./install/bcr_bot/share/bcr_bot/rviz/Maple.rviz

# open a new terminal in wsFolder
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