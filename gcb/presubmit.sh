#!/bin/bash
#
set -e

REMOTE_INSTANCE_NAME="projects/google.com:raksha-ci/instances/raksha_rbe_instance"
RESULT_UI="--bes_backend=buildeventservice.googleapis.com --bes_timeout=60s \
           --bes_results_url=https://source.cloud.google.com/results/invocations/ \
           --project_id=google.com:raksha-ci"
BAZEL_CONFIG="--config=remote --remote_instance_name=${REMOTE_INSTANCE_NAME} ${RESULT_UI}"
BAZELISK="bazelisk"

post_commit_status() {
  STATUS_CONTEXT=$1
  STATE=$2
  INVOCATION_ID=$3
  case "${STATE}" in
    "success")
      DESCRIPTION="Finished successfully!"
      ;;
    "failure")
      DESCRIPTION="Finished with errors!"
      ;;
    "pending")
      DESCRIPTION="Started..."
      ;;
    *)
      DESCRIPTION=""
      ;;
  esac
  TARGET_URL=https://source.cloud.google.com/results/invocations/${INVOCATION_ID}
  curl --user "${GITHUB_COMMIT_STATUS_TOKEN}" -X POST \
    -H "Accept: application/vnd.github.v3+json" \
    https://api.github.com/repos/google-research/${REPO_NAME}/statuses/${COMMIT_SHA} \
    -d "{ \
    \"state\": \"${STATE}\", \
    \"target_url\":\"${TARGET_URL}\", \
    \"description\":\"${DESCRIPTION}\", \
    \"context\": \"${STATUS_CONTEXT}\" \
    }"
}

# Run bazel. Post status commit messages before and after.
bazel_run() {
  TASK=$1; shift
  TARGETS=$*
  INVOCATION_ID=`uuidgen`
  STATUS_MESSAGE="Required ${TASK}s"
  post_commit_status "${STATUS_MESSAGE}" "pending" ${INVOCATION_ID}
  if ${BAZELISK} ${TASK} ${BAZEL_CONFIG} \
    --invocation_id=${INVOCATION_ID} \
    ${TARGETS};
  then
    post_commit_status "${STATUS_MESSAGE}" "success" ${INVOCATION_ID}
  else
    post_commit_status "${STATUS_MESSAGE}" "failure" ${INVOCATION_ID}
  fi
}

# Verifies that the following targets build fine:
#  - Arcs parser and proto works.
#  - Arcs manifest tests that we do not yet handle parse correctly.
#  - Arcs manifest examples.
bazel_run build \
  //third_party/arcs/examples:consume \
  //third_party/arcs/proto:manifest_cc_proto \
  //src/analysis/souffle/tests/arcs_manifest_tests_todo/... \
  //src/analysis/souffle/examples/...

# Run all the bazel tests (not cargo).
bazel_run test //src/...
