import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource


def generate_launch_description():
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
        ExecuteProcess(
            cmd=['ros2', 'bag', 'record', '-a', '-o', 'full_system_bag'],
            output='screen'
        )
    ])