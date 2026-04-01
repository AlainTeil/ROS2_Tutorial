"""Launch Gazebo with the SimpleBot arena world and spawn the robot."""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description() -> LaunchDescription:
    pkg_dir = get_package_share_directory("lesson_30_gazebo_world")
    world_file = os.path.join(pkg_dir, "worlds", "simplebot_arena.sdf")

    plugins_pkg = get_package_share_directory("lesson_28_gazebo_plugins")
    xacro_file = os.path.join(plugins_pkg, "urdf", "simplebot_gazebo.urdf.xacro")
    bridge_config = os.path.join(plugins_pkg, "config", "ros_gz_bridge.yaml")

    robot_description = ParameterValue(
        Command(["xacro ", xacro_file]), value_type=str
    )

    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("ros_gz_sim"),
                "launch",
                "gz_sim.launch.py",
            )
        ),
        launch_arguments={"gz_args": ["-r ", world_file]}.items(),
    )

    robot_state_pub = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description}],
        output="screen",
    )

    spawn = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=["-name", "simplebot", "-topic", "/robot_description", "-z", "0.05"],
        output="screen",
    )

    bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=["--ros-args", "-p", f"config_file:={bridge_config}"],
        output="screen",
    )

    return LaunchDescription([gz_sim, robot_state_pub, spawn, bridge])
