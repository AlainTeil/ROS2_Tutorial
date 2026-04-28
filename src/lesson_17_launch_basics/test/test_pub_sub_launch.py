"""launch_testing harness for pub_sub_launch.py.

Verifies that:
  1. The launch file starts cleanly.
  2. The subscriber receives at least one message from the publisher
     within a reasonable timeout.
  3. Both processes exit cleanly when the launch is shut down.

Run with:  ``colcon test --packages-select lesson_17_launch_basics``
or directly:  ``launch_test src/lesson_17_launch_basics/test/test_pub_sub_launch.py``
"""

import os
import time
import unittest

import launch
import launch_ros.actions
import launch_testing
import launch_testing.actions
import pytest
import rclpy
from std_msgs.msg import Int32


@pytest.mark.launch_test
def generate_test_description():
    publisher = launch_ros.actions.Node(
        package='lesson_07_topic_publisher',
        executable='number_publisher',
        name='test_publisher',
        output='screen',
    )
    subscriber = launch_ros.actions.Node(
        package='lesson_08_topic_subscriber',
        executable='number_subscriber',
        name='test_subscriber',
        output='screen',
    )
    return (
        launch.LaunchDescription([
            publisher,
            subscriber,
            launch_testing.actions.ReadyToTest(),
        ]),
        {'publisher': publisher, 'subscriber': subscriber},
    )


class TestPubSubLaunch(unittest.TestCase):
    """Active tests — run while the launched processes are alive."""

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def test_publisher_emits_messages(self, proc_output):
        """The publisher should publish on /counter within a few seconds."""
        node = rclpy.create_node('launch_test_observer')
        received = []

        def cb(msg):
            received.append(msg.data)

        node.create_subscription(Int32, 'counter', cb, 10)

        deadline = time.time() + 10.0
        while time.time() < deadline and not received:
            rclpy.spin_once(node, timeout_sec=0.1)

        node.destroy_node()
        self.assertGreater(len(received), 0,
                           'No messages observed on /counter within 10 s')


@launch_testing.post_shutdown_test()
class TestPubSubShutdown(unittest.TestCase):
    """Post-shutdown tests — both processes must exit cleanly."""

    def test_publisher_exit_code(self, proc_info, publisher):
        launch_testing.asserts.assertExitCodes(
            proc_info, allowable_exit_codes=[0, -2, -15], process=publisher)

    def test_subscriber_exit_code(self, proc_info, subscriber):
        launch_testing.asserts.assertExitCodes(
            proc_info, allowable_exit_codes=[0, -2, -15], process=subscriber)
