"""Launch Gazebo Harmonic with an empty world and spawn SimpleBot."""
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, ExecuteProcess
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description() -> LaunchDescription:
    pkg_dir = get_package_share_directory("lesson_27_gazebo_intro")
    world_file = os.path.join(pkg_dir, "worlds", "empty.sdf")

    xacro_pkg = get_package_share_directory("lesson_25_xacro_urdf")
    xacro_file = os.path.join(xacro_pkg, "urdf", "simplebot.urdf.xacro")

    robot_description = ParameterValue(
        Command(["xacro ", xacro_file]), value_type=str
    )

    # Start Gazebo Harmonic
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

    # robot_state_publisher for TF
    robot_state_pub = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description": robot_description}],
        output="screen",
    )

    # Spawn the robot in Gazebo
    spawn_entity = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            "-name", "simplebot",
            "-topic", "/robot_description",
            "-z", "0.05",
        ],
        output="screen",
    )

    # Bridge /clock from Gazebo to ROS2
    bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=["/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock"],
        output="screen",
    )

    return LaunchDescription([gz_sim, robot_state_pub, spawn_entity, bridge])
