#!/bin/bash
#
set -e
AUTH_LOGIC_DIR="rust/tools/authorization-logic"
cd ${AUTH_LOGIC_DIR}
cargo test
