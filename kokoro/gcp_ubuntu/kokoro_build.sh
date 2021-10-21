#!/bin/bash

# Fail on any error.
set -e

cd "${KOKORO_ARTIFACTS_DIR}/github/raksha"

# Work around an issue in GCP Ubuntu images.
pyenv init --path

# Install bazel.
use_bazel.sh 4.2.1 --quiet
command -v bazel
bazel version

bazel test //src/...
bazel build //third_party/arcs/examples:consume third_party/arcs/proto:manifest_cc_proto
bazel build //src/analysis/souffle/tests/arcs_manifest_tests_todo/...
bazel build //rust/tools/authorization-logic:authorization_logic
