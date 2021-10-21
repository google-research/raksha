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

bazel build ...
bazel test ...
