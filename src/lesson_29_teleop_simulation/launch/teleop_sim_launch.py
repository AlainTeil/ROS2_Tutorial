"""Launch Gazebo + teleop node + sensor display."""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    # Reuse the lesson_28 Gazebo + plugin launch
    gz_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("lesson_28_gazebo_plugins"),
                "launch",
                "gazebo_simplebot_launch.py",
            )
        )
    )

    teleop = Node(
        package="lesson_29_teleop_simulation",
        executable="teleop_node",
        name="teleop_node",
        output="screen",
    )

    sensor_display = Node(
        package="lesson_29_teleop_simulation",
        executable="sensor_display_node",
        name="sensor_display",
        output="screen",
    )

    return LaunchDescription([gz_launch, teleop, sensor_display])
