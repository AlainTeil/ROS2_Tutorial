"""Description-only smoke test for gazebo_arena_launch.py.

The full launch requires a running Gazebo server (not available in
headless CI). This test imports the launch file and verifies that
`generate_launch_description()` returns a well-formed tree, catching
package-share path errors and Python syntax regressions.
"""

import os
import runpy
import unittest

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription


class TestGazeboArenaLaunchDescription(unittest.TestCase):

    def test_generate_launch_description(self):
        pkg_share = get_package_share_directory("lesson_30_gazebo_world")
        launch_path = os.path.join(pkg_share, "launch", "gazebo_arena_launch.py")
        self.assertTrue(os.path.isfile(launch_path), launch_path)

        ns = runpy.run_path(launch_path)
        ld = ns["generate_launch_description"]()
        self.assertIsInstance(ld, LaunchDescription)
        # gz_sim + robot_state_publisher + spawn + bridge => 4 entities.
        self.assertGreaterEqual(len(ld.entities), 4)


if __name__ == "__main__":
    unittest.main()
