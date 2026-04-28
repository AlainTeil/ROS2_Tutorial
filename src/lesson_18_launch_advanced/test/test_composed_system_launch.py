"""launch_testing harness for composed_system_launch.py.

Verifies that:
  1. The advanced launch file starts cleanly with default arguments.
  2. The publisher emits messages on the namespaced topic
     (`/<robot_ns>/counter`) within a few seconds.
  3. All processes exit cleanly when the launch is shut down.
"""

import time
import unittest

import launch
import launch.actions
import launch.launch_description_sources
import launch_testing
import launch_testing.actions
import launch_testing.asserts
import pytest
import rclpy
from ament_index_python.packages import get_package_share_directory
from std_msgs.msg import Int32


@pytest.mark.launch_test
def generate_test_description():
    pkg_share = get_package_share_directory("lesson_18_launch_advanced")
    composed = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            f"{pkg_share}/launch/composed_system_launch.py"
        ),
        launch_arguments={
            "robot_ns": "test_bot",
            "enable_controller": "false",
        }.items(),
    )
    return launch.LaunchDescription([
        composed,
        launch_testing.actions.ReadyToTest(),
    ])


class TestComposedLaunch(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def test_namespaced_topic_publishes(self, proc_output):
        """The publisher should emit on /test_bot/counter under the namespace."""
        node = rclpy.create_node("composed_launch_test_observer")
        received = []
        node.create_subscription(
            Int32, "/test_bot/counter", lambda m: received.append(m.data), 10
        )

        deadline = time.time() + 10.0
        while time.time() < deadline and not received:
            rclpy.spin_once(node, timeout_sec=0.1)

        node.destroy_node()
        self.assertGreater(
            len(received), 0,
            "No messages observed on /test_bot/counter within 10 s "
            "- the namespace push or the publisher node is broken."
        )


@launch_testing.post_shutdown_test()
class TestComposedShutdown(unittest.TestCase):

    def test_all_processes_clean_exit(self, proc_info):
        # SIGINT (-2) and SIGTERM (-15) are the launch shutdown signals.
        launch_testing.asserts.assertExitCodes(
            proc_info, allowable_exit_codes=[0, -2, -15]
        )
