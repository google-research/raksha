#!/bin/bash
set -e
AUTH_LOGIC_DIR="rust/tools/authorization-logic"
cd ${AUTH_LOGIC_DIR}
cp -r /chef_tmp/rust/tools/authorization-logic/target .
cargo test
