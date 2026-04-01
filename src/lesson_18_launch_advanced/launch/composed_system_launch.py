"""Advanced launch file demonstrating composition, conditions, and namespaces."""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    GroupAction,
    IncludeLaunchDescription,
)
from launch.conditions import IfCondition, UnlessCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node, PushRosNamespace


def generate_launch_description():
    # --- Declare arguments ---
    use_sim_arg = DeclareLaunchArgument(
        'use_sim', default_value='false',
        description='Enable simulation mode')

    namespace_arg = DeclareLaunchArgument(
        'robot_ns', default_value='simplebot',
        description='Robot namespace')

    enable_controller_arg = DeclareLaunchArgument(
        'enable_controller', default_value='true',
        description='Start the robot controller node')

    # --- Parameter file ---
    pkg_dir = get_package_share_directory('lesson_18_launch_advanced')
    params_file = os.path.join(pkg_dir, 'config', 'default_params.yaml')

    # --- Grouped nodes under a namespace ---
    robot_group = GroupAction([
        PushRosNamespace(LaunchConfiguration('robot_ns')),

        # Publisher (always started)
        Node(
            package='lesson_07_topic_publisher',
            executable='number_publisher',
            name='number_publisher',
            output='screen',
        ),

        # Subscriber (always started)
        Node(
            package='lesson_08_topic_subscriber',
            executable='number_subscriber',
            name='number_subscriber',
            output='screen',
        ),

        # Controller (conditional)
        Node(
            package='lesson_15_parameters',
            executable='robot_controller',
            name='robot_controller',
            parameters=[params_file],
            output='screen',
            condition=IfCondition(LaunchConfiguration('enable_controller')),
        ),
    ])

    return LaunchDescription([
        use_sim_arg,
        namespace_arg,
        enable_controller_arg,
        robot_group,
    ])
