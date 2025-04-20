import os
import datetime

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource


def generate_launch_description():
    realsense_launch_file = os.path.join(
        get_package_share_directory("realsense2_camera"), "launch", "rs_launch.py"
    )
    hokuyo_launch_file = os.path.join(
        get_package_share_directory("urg3d_node2"), "launch", "urg3d_node2.launch.py"
    )

    realsense = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(realsense_launch_file)
    )

    lidar = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(hokuyo_launch_file)
    )
    bags = ExecuteProcess(
        cmd=['ros2', 'bag', 'record', '-o', '/home/roboracing/bagfiles/test-ros-bag-' +  str(datetime.datetime.now()).replace(" ", "").replace(".", "_").replace(":", "-"), '-a'],
        output='screen'
    )
    
    vision_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
            get_package_share_directory('vision'),
            'launch',
            'vision_launch.py'
            )
        )
    )

    comms_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
            get_package_share_directory('comms'),
            'launch',
            'comms_launch.py'
            )
        )
    )

    wall_follower_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
            get_package_share_directory('wall_follower'),
            'launch',
            'wall_follower_launch.py'
            )
        )
    )

    return LaunchDescription([
        vision_node,
        comms_node,
        wall_follower_node,
        lidar,
        realsense,
        bags
    ])