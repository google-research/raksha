#!/bin/bash
#
# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-------------------------------------------------------------------------------

# A simple script to test the generate_datalog_program command line.
ROOT_DIR=$TEST_SRCDIR/$TEST_WORKSPACE/src/frontends/arcs/xform_to_datalog
CMD=$ROOT_DIR/generate_datalog_program

AUTH_FILE=$ROOT_DIR/testdata/ok_claim_propagates.auth
MANIFEST_FILE=$ROOT_DIR/testdata/ok_claim_propagates_proto.binarypb
DATALOG_FILE=$ROOT_DIR/testdata/ok_claim_propagates.dl
GENERATED_DATALOG_FILE=`mktemp`

$CMD --auth_logic_file=$AUTH_FILE --manifest_proto=$MANIFEST_FILE \
  --datalog_file=$GENERATED_DATALOG_FILE --overwrite

# Return the result of comparing generated and golden file.
diff $GENERATED_DATALOG_FILE $DATALOG_FILE
