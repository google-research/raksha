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

BAZEL_OPTS=--cxxopt='-std=c++17'--host_cxxopt='-std=c++17' --cxxopt='-Werror' --host_cxxopt='-Werror' --cxxopt='-Wall' --host_cxxopt='-Wall' --cxxopt='-Wno-deprecated-declarations' --host_cxxopt='-Wno-deprecated-declarations'

bazel test ${BAZEL_OPTS} //src/...
bazel build ${BAZEL_OPTS} //third_party/arcs/examples:consume third_party/arcs/proto:manifest_cc_proto
bazel build ${BAZEL_OPTS} //src/analysis/souffle/tests/arcs_manifest_tests_todo/...
bazel build ${BAZEL_OPTS} //rust/tools/authorization-logic:authorization_logic
