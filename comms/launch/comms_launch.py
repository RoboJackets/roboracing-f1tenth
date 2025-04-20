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
            package='comms',
            namespace='comms',
            executable='ackermann_sned',
            name='comms'
        ),
        Node(
            package='comms',
            namespace='comms',
            executable='udp_client',
            name='comms'
        )
    ])