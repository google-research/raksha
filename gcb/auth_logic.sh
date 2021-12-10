#!/bin/bash
#
set -e
AUTH_LOGIC_DIR="rust/tools/authorization-logic"
cd ${AUTH_LOGIC_DIR}
echo "pwd:"
pwd
echo "ls /chef_tmp/rust/tools/authorization-logic/target:"
ls /chef_tmp/rust/tools/authorization-logic/target
echo "copy target:"
cp -r /chef_tmp/rust/tools/authorization-logic/target .
echo "ls target:"
ls target
echo "starting test"
cargo test
