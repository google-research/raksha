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

bazel --nosystem_rc --nohome_rc test //src/...
bazel --nosystem_rc --nohome_rc build //third_party/arcs/examples:consume third_party/arcs/proto:manifest_cc_proto
bazel --nosystem_rc --nohome_rc build //src/analysis/souffle/tests/arcs_manifest_tests_todo/...
bazel --nosystem_rc --nohome_rc build //rust/tools/authorization-logic:authorization_logic
