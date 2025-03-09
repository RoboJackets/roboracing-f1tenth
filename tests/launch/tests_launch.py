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
        Node(
            package='tests',
            namespace='tests',
            executable='realsense_camera_node',
            name='tests'
        ),
        Node(
            package='tests',
            namespace='tests',
            executable='depth_node',
            name='tests'
        ),
        Node(
            package='tests',
            namespace='tests',
            executable='cone_detection_node',
            name='tests'
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(realsense_launch_file)
        )
    ])