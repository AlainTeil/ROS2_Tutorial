"""launch_testing harness for capstone_launch.py.

Verifies that:
  1. The capstone lifecycle node launches cleanly.
  2. The node reports an `lifecycle_msgs/srv/GetState` service - the
     canonical signal that a managed lifecycle node has reached the
     `unconfigured` state and is ready to be configured.
  3. The process exits cleanly on shutdown.
"""

import time
import unittest

import launch
import launch_testing
import launch_testing.actions
import launch_testing.asserts
import launch_ros.actions
import pytest
import rclpy
from lifecycle_msgs.srv import GetState


@pytest.mark.launch_test
def generate_test_description():
    capstone_node = launch_ros.actions.LifecycleNode(
        package="lesson_32_going_further",
        executable="capstone_robot_node",
        name="capstone_robot",
        namespace="",
        output="screen",
    )
    return (
        launch.LaunchDescription([
            capstone_node,
            launch_testing.actions.ReadyToTest(),
        ]),
        {"capstone": capstone_node},
    )


class TestCapstoneActive(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def test_lifecycle_get_state_responds(self, proc_output):
        """The capstone node should answer /capstone_robot/get_state."""
        node = rclpy.create_node("capstone_launch_test_client")
        client = node.create_client(GetState, "/capstone_robot/get_state")
        ready = client.wait_for_service(timeout_sec=10.0)
        self.assertTrue(
            ready,
            "/capstone_robot/get_state never appeared - the lifecycle "
            "node failed to start within 10 s."
        )

        future = client.call_async(GetState.Request())
        deadline = time.time() + 5.0
        while time.time() < deadline and not future.done():
            rclpy.spin_once(node, timeout_sec=0.1)
        node.destroy_node()

        self.assertTrue(future.done(), "GetState call did not complete")
        # PRIMARY_STATE_UNCONFIGURED == 1; PRIMARY_STATE_INACTIVE == 2.
        self.assertIn(future.result().current_state.id, (1, 2))


@launch_testing.post_shutdown_test()
class TestCapstoneShutdown(unittest.TestCase):

    def test_capstone_clean_exit(self, proc_info, capstone):
        launch_testing.asserts.assertExitCodes(
            proc_info, allowable_exit_codes=[0, -2, -15], process=capstone
        )
