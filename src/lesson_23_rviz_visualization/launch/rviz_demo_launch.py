"""Launch marker publisher and RViz2 with a preconfigured view."""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    pkg_dir = get_package_share_directory("lesson_23_rviz_visualization")
    rviz_config = os.path.join(pkg_dir, "rviz", "waypoints.rviz")

    marker_node = Node(
        package="lesson_23_rviz_visualization",
        executable="marker_publisher_node",
        name="marker_publisher",
        output="screen",
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        arguments=["-d", rviz_config],
        output="screen",
    )

    return LaunchDescription([marker_node, rviz_node])
