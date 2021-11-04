#!/bin/bash
#
set -e

REMOTE_INSTANCE_NAME="projects/google.com:raksha-ci/instances/raksha_rbe_instance"
RESULT_UI="--bes_backend=buildeventservice.googleapis.com --bes_timeout=60s \
           --bes_results_url=https://source.cloud.google.com/results/invocations/ \
           --project_id=google.com:raksha-ci"
BAZEL_TEST="bazelisk test --config=remote --remote_instance_name=${REMOTE_INSTANCE_NAME} ${RESULT_UI}"
BAZEL_BUILD="bazelisk build --config=remote --remote_instance_name=${REMOTE_INSTANCE_NAME} ${RESULT_UI}"

# Verifies that the following targets build fine:
#  - Arcs parser and proto works.
#  - Arcs manifest tests that we do not yet handle parse correctly.
#  - Arcs manifest examples.
${BAZEL_BUILD} //third_party/arcs/examples:consume third_party/arcs/proto:manifest_cc_proto \
   //src/analysis/souffle/tests/arcs_manifest_tests_todo/... \
   //src/analysis/souffle/examples/...

# Run all the tests.
${BAZEL_TEST} //src/...
