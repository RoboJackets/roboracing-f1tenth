# Steps to run a package in the F1 Tenth Sim

1. Launch the container
```
docker-compose up
```
2. Open a terminal connected to the container with 
```
docker exec -it f1tenth_gym_ros-sim-1 /bin/bash
```

3. Navigate to localhost:8080 to get the noVNC window.

4. Before you can run the sim, you need to source setup files with the following commands in /sim_ws
```
source /opt/ros/foxy/setup.bash
source install/setup.bash
```
(The source command for ROS can be added to the .bashrc file for convenience)

5. Navigate to the package you want to run. After running colcon build, there will be an install folder with another setup file to source
```
source install/setup.bash
```

6. To run the ros node, use the following command
```
ros2 run <package> <executable> 
```
