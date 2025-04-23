from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

import os

def generate_launch_description():
    realsense_launch_file = os.path.join(
        get_package_share_directory("realsense2_camera"), "launch", "rs_launch.py"
    )
    return LaunchDescription([
        # Node(
        #     package='vision',
        #     namespace='vision',
        #     executable='cone',
        #     name='cone'
        # ),
        # Node(
        #     package='vision',
        #     namespace='vision',
        #     executable='video',
        #     name='video'
        # ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(realsense_launch_file)
        )
    ])