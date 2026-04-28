"""Description-only smoke test for display_robot_launch.py.

The full launch starts `joint_state_publisher_gui` and `rviz2`, both of
which require a display server. CI is headless, so we instead import the
launch module and call `generate_launch_description()` directly. This
catches:
  * Python import errors in the launch file
  * Missing `get_package_share_directory()` lookups (e.g. the xacro file
    moved or the rviz config was renamed)
  * Syntactic regressions in the LaunchDescription tree

It does NOT verify that GUI nodes actually start - that is intentional
and out of scope for headless CI.
"""

import os
import unittest

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription


class TestDisplayRobotLaunchDescription(unittest.TestCase):

    def test_generate_launch_description_returns_valid_tree(self):
        pkg_share = get_package_share_directory("lesson_26_robot_state_publisher")
        launch_path = os.path.join(pkg_share, "launch", "display_robot_launch.py")
        self.assertTrue(os.path.isfile(launch_path),
                        f"launch file not installed: {launch_path}")

        # Import via runpy so we do not pollute sys.modules with a fixed name.
        import runpy
        ns = runpy.run_path(launch_path)
        self.assertIn("generate_launch_description", ns,
                      "launch file is missing generate_launch_description()")

        ld = ns["generate_launch_description"]()
        self.assertIsInstance(ld, LaunchDescription)
        self.assertGreaterEqual(len(ld.entities), 3,
                                "expected robot_state_publisher + jsp_gui + rviz")


if __name__ == "__main__":
    unittest.main()
