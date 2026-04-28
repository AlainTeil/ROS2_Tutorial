#!/usr/bin/env bash
# Build the workspace with coverage instrumentation, run the full test
# suite, and produce an lcov HTML report under coverage-html/.
#
# Usage:
#   ./scripts/coverage.sh            # full clean coverage run
#   ./scripts/coverage.sh --keep     # reuse existing build/install/log
#
# Requirements: lcov, gcov, colcon, ROS 2 Jazzy sourced or installed.
set -euo pipefail

cd "$(dirname "$0")/.."

KEEP=0
if [[ "${1:-}" == "--keep" ]]; then
  KEEP=1
fi

if ! command -v lcov >/dev/null; then
  echo "lcov not found - install with: sudo apt-get install -y lcov" >&2
  exit 2
fi

if [[ -z "${AMENT_PREFIX_PATH:-}" ]]; then
  if [[ -f /opt/ros/jazzy/setup.bash ]]; then
    # ROS setup scripts reference unset vars; relax nounset while sourcing.
    set +u
    # shellcheck disable=SC1091
    source /opt/ros/jazzy/setup.bash
    set -u
  else
    echo "ROS 2 environment not sourced and /opt/ros/jazzy not found" >&2
    exit 2
  fi
fi

if [[ $KEEP -eq 0 ]]; then
  rm -rf build install log coverage.info coverage-html
fi

# Coverage flags: -O0 keeps line numbers stable; --coverage adds the
# gcov instrumentation.
COV_FLAGS="-O0 -g --coverage -fprofile-arcs -ftest-coverage"

colcon build \
  --cmake-args \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="${COV_FLAGS}" \
    -DCMAKE_C_FLAGS="${COV_FLAGS}" \
    -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
    -DCMAKE_SHARED_LINKER_FLAGS="--coverage" \
  --event-handlers console_cohesion+

# colcon's generated setup.bash references $COLCON_TRACE without a default,
# which trips `set -u`. Relax nounset just for the source.
set +u
# shellcheck disable=SC1091
source install/setup.bash
set -u

colcon test --event-handlers console_cohesion+ || true
colcon test-result --verbose

# Capture coverage from build/, then strip system + test + generated code.
lcov --capture \
  --directory build \
  --output-file coverage.info \
  --rc geninfo_unexecuted_blocks=1 \
  --ignore-errors mismatch,gcov,negative,unused,empty

lcov --remove coverage.info \
  '/usr/*' \
  '/opt/ros/*' \
  '*/test/*' \
  '*/build/*/rosidl_generator_*/*' \
  '*/build/*/CMakeFiles/*' \
  '*/install/*' \
  --output-file coverage.info \
  --ignore-errors unused,empty

lcov --list coverage.info

if command -v genhtml >/dev/null; then
  genhtml coverage.info \
    --output-directory coverage-html \
    --title "ROS2 Tutorial Coverage" \
    --legend \
    --demangle-cpp \
    --ignore-errors source
  echo "HTML report: coverage-html/index.html"
fi
