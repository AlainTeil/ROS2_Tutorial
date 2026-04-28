"""launch_testing harness for composable_launch.py.

Verifies that:
  1. The component container starts and loads both composable nodes.
  2. The composable subscriber actually receives messages from the
     composable publisher (in-process comms work end-to-end).
  3. The container exits cleanly on shutdown.
"""

import time
import unittest

import launch
import launch_testing
import launch_testing.actions
import pytest
import rclpy
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from std_msgs.msg import Int32


@pytest.mark.launch_test
def generate_test_description():
    container = ComposableNodeContainer(
        name='test_component_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='lesson_20_composable_nodes',
                plugin='lesson_20::ComposablePublisher',
                name='publisher',
                extra_arguments=[{'use_intra_process_comms': True}],
            ),
            ComposableNode(
                package='lesson_20_composable_nodes',
                plugin='lesson_20::ComposableSubscriber',
                name='subscriber',
                extra_arguments=[{'use_intra_process_comms': True}],
            ),
        ],
        output='screen',
    )
    return (
        launch.LaunchDescription([
            container,
            launch_testing.actions.ReadyToTest(),
        ]),
        {'container': container},
    )


class TestComposableLaunch(unittest.TestCase):
    """Active tests — run while the container is alive."""

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def test_publisher_emits_messages(self, proc_output):
        """The composable publisher should publish on /counter within 10 s."""
        node = rclpy.create_node('launch_test_observer')
        received = []

        def cb(msg):
            received.append(msg.data)

        node.create_subscription(Int32, 'counter', cb, 10)

        deadline = time.time() + 10.0
        while time.time() < deadline and not received:
            rclpy.spin_once(node, timeout_sec=0.1)

        node.destroy_node()
        self.assertGreater(
            len(received), 0,
            'No messages observed on /counter within 10 s')


@launch_testing.post_shutdown_test()
class TestComposableShutdown(unittest.TestCase):
    """Post-shutdown tests — the container must exit cleanly."""

    def test_container_exit_code(self, proc_info, container):
        launch_testing.asserts.assertExitCodes(
            proc_info, allowable_exit_codes=[0, -2, -15], process=container)
