"""Launch robot_state_publisher + joint_state_publisher_gui + RViz2."""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.substitutions import Command
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description() -> LaunchDescription:
    xacro_pkg = get_package_share_directory("lesson_25_xacro_urdf")
    xacro_file = os.path.join(xacro_pkg, "urdf", "simplebot.urdf.xacro")

    pkg_dir = get_package_share_directory("lesson_26_robot_state_publisher")
    rviz_config = os.path.join(pkg_dir, "rviz", "simplebot.rviz")

    robot_description = ParameterValue(
        Command(["xacro ", xacro_file]), value_type=str
    )

    robot_state_pub = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description}],
        output="screen",
    )

    joint_state_pub_gui = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui",
        output="screen",
    )

    rviz = Node(
        package="rviz2",
        executable="rviz2",
        arguments=["-d", rviz_config],
        output="screen",
    )

    return LaunchDescription([robot_state_pub, joint_state_pub_gui, rviz])
