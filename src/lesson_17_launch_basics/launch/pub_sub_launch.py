"""Launch file for publisher + subscriber from Lessons 07 and 08."""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    topic_arg = DeclareLaunchArgument(
        'topic', default_value='counter',
        description='Topic name for publisher and subscriber')

    publisher_node = Node(
        package='lesson_07_topic_publisher',
        executable='number_publisher',
        name='number_publisher',
        remappings=[('counter', LaunchConfiguration('topic'))],
        output='screen',
    )

    subscriber_node = Node(
        package='lesson_08_topic_subscriber',
        executable='number_subscriber',
        name='number_subscriber',
        remappings=[('counter', LaunchConfiguration('topic'))],
        output='screen',
    )

    return LaunchDescription([
        topic_arg,
        publisher_node,
        subscriber_node,
    ])
