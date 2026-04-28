"""Description-only smoke test for rviz_demo_launch.py.

The full launch starts `rviz2`, which requires a display server and is
not available on headless CI runners. We therefore import the launch
module and call `generate_launch_description()` directly. This catches:
  * Python import errors in the launch file
  * Missing `get_package_share_directory()` lookups (e.g. the rviz
    config was renamed or moved)
  * Syntactic regressions in the LaunchDescription tree

It does NOT verify that RViz actually starts — that is intentional and
out of scope for headless CI.
"""

import os
import runpy
import unittest

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription


class TestRvizDemoLaunchDescription(unittest.TestCase):

    def test_generate_launch_description_returns_valid_tree(self):
        pkg_share = get_package_share_directory('lesson_23_rviz_visualization')
        launch_path = os.path.join(pkg_share, 'launch', 'rviz_demo_launch.py')
        self.assertTrue(os.path.isfile(launch_path),
                        f'launch file not installed: {launch_path}')

        ns = runpy.run_path(launch_path)
        self.assertIn('generate_launch_description', ns,
                      'launch file is missing generate_launch_description()')

        ld = ns['generate_launch_description']()
        self.assertIsInstance(ld, LaunchDescription)
        self.assertGreaterEqual(len(ld.entities), 2,
                                'expected marker_publisher + rviz2')


if __name__ == '__main__':
    unittest.main()
