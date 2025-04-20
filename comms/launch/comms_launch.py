from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

import os

def generate_launch_description():
    return LaunchDescription([
        # Node(
        #     package='comms',
        #     namespace='comms',
        #     executable='ackermann_send',
        #     name='ackermann_sender',
        #     output='screen'
        # ),
        Node(
            package='comms',
            namespace='comms',
            executable='udp',
            name='udp_node',
            output='screen'
        ),
    ])