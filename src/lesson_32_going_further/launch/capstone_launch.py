"""Launch the capstone lifecycle node."""
from launch import LaunchDescription
from launch_ros.actions import LifecycleNode


def generate_launch_description() -> LaunchDescription:
    capstone_node = LifecycleNode(
        package="lesson_32_going_further",
        executable="capstone_robot_node",
        name="capstone_robot",
        namespace="",
        output="screen",
        parameters=[
            {"patrol_speed": 0.5},
            {"heartbeat_period": 1.0},
        ],
    )

    return LaunchDescription([capstone_node])
