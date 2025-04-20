from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import datetime

import os

def generate_launch_description():
    realsense_launch_file = os.path.join(
        get_package_share_directory("realsense2_camera"), "launch", "rs_launch.py"
    )
    hokuyo_launch_file = os.path.join(
        get_package_share_directory("urg3d_node2"), "launch", "urg3d_node2.launch.py"
    )
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
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(realsense_launch_file)
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(hokuyo_launch_file)
        ),
        ExecuteProcess(
            cmd=['ros2', 'bag', 'record', '-o', '/home/roboracing/bagfiles/test-ros-bag-' +  str(datetime.datetime.now()).replace(" ", "").replace(".", "_").replace(":", "-"), '-a'],
            output='screen'
        )
    ])