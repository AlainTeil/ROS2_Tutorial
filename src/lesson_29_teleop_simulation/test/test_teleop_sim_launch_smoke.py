"""Description-only smoke test for teleop_sim_launch.py.

The full launch requires a running Gazebo server, which is not available
in headless CI. This test imports the launch file and verifies that
`generate_launch_description()` returns a well-formed tree.
"""

import os
import runpy
import unittest

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription


class TestTeleopSimLaunchDescription(unittest.TestCase):

    def test_generate_launch_description(self):
        pkg_share = get_package_share_directory("lesson_29_teleop_simulation")
        launch_path = os.path.join(pkg_share, "launch", "teleop_sim_launch.py")
        self.assertTrue(os.path.isfile(launch_path), launch_path)

        ns = runpy.run_path(launch_path)
        ld = ns["generate_launch_description"]()
        self.assertIsInstance(ld, LaunchDescription)
        self.assertGreater(len(ld.entities), 0)


if __name__ == "__main__":
    unittest.main()
