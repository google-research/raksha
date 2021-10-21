#!/bin/bash

# Fail on any error.
set -e

cd "${KOKORO_ARTIFACTS_DIR}/github/raksha"

# Install bazel.
use_bazel.sh latest --quiet
command -v bazel
bazel version

bazel build ...
bazel test ...
