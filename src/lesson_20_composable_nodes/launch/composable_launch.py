"""Launch composable publisher and subscriber in a single container."""
from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


def generate_launch_description() -> LaunchDescription:
    container = ComposableNodeContainer(
        name="component_container",
        namespace="",
        package="rclcpp_components",
        executable="component_container",
        composable_node_descriptions=[
            ComposableNode(
                package="lesson_20_composable_nodes",
                plugin="lesson_20::ComposablePublisher",
                name="publisher",
                extra_arguments=[{"use_intra_process_comms": True}],
            ),
            ComposableNode(
                package="lesson_20_composable_nodes",
                plugin="lesson_20::ComposableSubscriber",
                name="subscriber",
                extra_arguments=[{"use_intra_process_comms": True}],
            ),
        ],
        output="screen",
    )
    return LaunchDescription([container])
