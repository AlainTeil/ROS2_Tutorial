#!/usr/bin/env bash
# Run cppcheck across all lesson packages with shared suppressions.
#
# Usage:
#   ./scripts/run_cppcheck.sh           # run, fail on any finding
#   ./scripts/run_cppcheck.sh --soft    # run, do not fail (for local triage)
#
# Exit codes:
#   0 - no findings (or --soft)
#   1 - findings reported
set -euo pipefail

cd "$(dirname "$0")/.."

SOFT=0
if [[ "${1:-}" == "--soft" ]]; then
  SOFT=1
fi

# Discover include dirs automatically: any src/lesson_*/include directory.
INCLUDE_ARGS=()
while IFS= read -r d; do
  INCLUDE_ARGS+=("-I" "$d")
done < <(find src -maxdepth 3 -type d -name include | sort)

# Suppressions:
#  - useInitializationList: tutorial code intentionally uses constructor
#    bodies for create_publisher/create_client/etc. so the steps are
#    explicit for learners.
#  - unusedStructMember: GTest fixture members are instantiated per test
#    even when not directly read.
#  - missingInclude*: cppcheck does not see the ROS install tree.
#  - normalCheckLevelMaxBranches: noisy on generated message headers.
SUPPRESSIONS=(
  --suppress=useInitializationList
  --suppress=unusedStructMember
  --suppress=missingInclude
  --suppress=missingIncludeSystem
  --suppress=unusedFunction
  --suppress=unmatchedSuppression
  --suppress=useStlAlgorithm
  --suppress=normalCheckLevelMaxBranches
)

set +e
cppcheck \
  --enable=warning,style,performance,portability \
  --inline-suppr \
  "${SUPPRESSIONS[@]}" \
  --error-exitcode=2 \
  --quiet \
  --std=c++20 \
  --language=c++ \
  "${INCLUDE_ARGS[@]}" \
  src/ 2>&1 | tee cppcheck-report.txt
RC=${PIPESTATUS[0]}
set -e

if [[ $RC -ne 0 ]] && [[ $SOFT -eq 0 ]]; then
  echo "::error::cppcheck reported findings (exit=$RC) — see cppcheck-report.txt"
  exit 1
fi
exit 0
