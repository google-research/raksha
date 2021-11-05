#!/bin/bash
#
set -e

REMOTE_INSTANCE_NAME="projects/google.com:raksha-ci/instances/raksha_rbe_instance"
RESULT_UI="--bes_backend=buildeventservice.googleapis.com --bes_timeout=60s \
           --bes_results_url=https://source.cloud.google.com/results/invocations/ \
           --project_id=google.com:raksha-ci"
BAZEL_TEST="bazelisk test --config=remote --remote_instance_name=${REMOTE_INSTANCE_NAME} ${RESULT_UI}"
BAZEL_BUILD="bazelisk build --config=remote --remote_instance_name=${REMOTE_INSTANCE_NAME} ${RESULT_UI}"

post_commit_status() {
  STATUS_CONTEXT=$1
  STATE=$2
  INVOCATION_ID=$3
  TARGET_URL=https://source.cloud.google.com/results/invocations/${INVOCATION_ID}
  curl --user "${GITHUB_COMMIT_STATUS_TOKEN}" -X POST \
    -H "Accept: application/vnd.github.v3+json" \
    https://api.github.com/repos/google-research/${REPO_NAME}/statuses/${COMMIT_SHA} \
    -d "{\"state\": \"${STATE}\", \"target_url\":\"${TARGET_URL}\", \"context\": \"${STATUS_CONTEXT}\"}"
}

# Verifies that the following targets build fine:
#  - Arcs parser and proto works.
#  - Arcs manifest tests that we do not yet handle parse correctly.
#  - Arcs manifest examples.
BUILD_INVOCATION_ID=`uuidgen`
BUILD_STATUS_MESSAGE="Required Builds"
post_commit_status "${BUILD_STATUS_MESSAGE}" "pending" ${BUILD_INVOCATION_ID}
if ${BAZEL_BUILD} --invocation_id=${BUILD_INVOCATION_ID} \
  //third_party/arcs/examples:consume third_party/arcs/proto:manifest_cc_proto \
  //src/analysis/souffle/tests/arcs_manifest_tests_todo/... \
  //src/analysis/souffle/examples/... ; then
  post_commit_status "${BUILD_STATUS_MESSAGE}" "success" ${BUILD_INVOCATION_ID}
else
  post_commit_status "${BUILD_STATUS_MESSAGE}" "failure" ${BUILD_INVOCATION_ID}
fi

# Run all the tests.
TEST_INVOCATION_ID=`uuidgen`
TEST_STATUS_MESSAGE="Required Tests"
post_commit_status "${TEST_STATUS_MESSAGE}" "pending" ${TEST_INVOCATION_ID}
if ${BAZEL_TEST} --invocation_id=${TEST_INVOCATION_ID} //src/... ;
then
  post_commit_status "${TEST_STATUS_MESSAGE}" "success" ${TEST_INVOCATION_ID}
else
  post_commit_status "${TEST_STATUS_MESSAGE}" "failure" ${TEST_INVOCATION_ID}
fi
