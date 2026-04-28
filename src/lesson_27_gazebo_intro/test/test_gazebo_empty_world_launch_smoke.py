"""Description-only smoke test for gazebo_empty_world_launch.py.

The full launch starts the Gazebo Harmonic simulator, which is too heavy
to spin up on a default GitHub Actions runner. Instead we import the
launch module and call `generate_launch_description()` directly to
catch:
  * Python import errors in the launch file
  * Missing `get_package_share_directory()` lookups (e.g. the world or
    xacro file was renamed)
  * Syntactic regressions in the LaunchDescription tree
"""

import os
import runpy
import unittest

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription


class TestGazeboEmptyWorldLaunchDescription(unittest.TestCase):

    def test_generate_launch_description_returns_valid_tree(self):
        pkg_share = get_package_share_directory('lesson_27_gazebo_intro')
        launch_path = os.path.join(
            pkg_share, 'launch', 'gazebo_empty_world_launch.py')
        self.assertTrue(os.path.isfile(launch_path),
                        f'launch file not installed: {launch_path}')

        ns = runpy.run_path(launch_path)
        self.assertIn('generate_launch_description', ns,
                      'launch file is missing generate_launch_description()')

        ld = ns['generate_launch_description']()
        self.assertIsInstance(ld, LaunchDescription)
        self.assertGreaterEqual(
            len(ld.entities), 4,
            'expected gz_sim + robot_state_publisher + spawn + clock bridge')


if __name__ == '__main__':
    unittest.main()
