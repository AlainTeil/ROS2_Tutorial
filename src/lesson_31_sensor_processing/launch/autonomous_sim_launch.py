"""Launch Gazebo arena + obstacle avoidance node."""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    arena_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("lesson_30_gazebo_world"),
                "launch",
                "gazebo_arena_launch.py",
            )
        )
    )

    avoidance = Node(
        package="lesson_31_sensor_processing",
        executable="obstacle_avoidance_node",
        name="obstacle_avoidance",
        output="screen",
    )

    return LaunchDescription([arena_launch, avoidance])
