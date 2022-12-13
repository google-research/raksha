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

function printUsageAndExit() {
  echo "Usage: "
  echo "  test_authlogic.sh \ "
  echo "     <auth_logic_binary> <auth_logic_file> <expected_datalog_file> <test_queries> <expected_queries>"
  exit 1
}

CMD_ARG=$1
AUTH_LOGIC_FILE_ARG=$2
EXPECTED_DATALOG_FILE=$3
TEST_QUERIES=${4:-0}
EXPECTED_QUERIES_FILE=${5:-0}
OUTPUT_SOUFFLE_FILE_ARG=$(mktemp)
OUTPUT_QUERY_DIR_ARG=$(mktemp -d)

#Generates datalog file for given authorization logic
QUERY_TEST_RESULT=1
$CMD_ARG  --policy_rules=$AUTH_LOGIC_FILE_ARG  --datalog_file=$OUTPUT_SOUFFLE_FILE_ARG --skip_declarations=true

if [ -z "$SOUFFLE_BIN" ]
then
        echo "Set SOUFFLE_BIN"
        exit 1
else
        echo $SOUFFLE_BIN
fi

#Populates queries in .csv
PATH=$PATH:third_party/mcpp
$SOUFFLE_BIN $OUTPUT_SOUFFLE_FILE_ARG -D $OUTPUT_QUERY_DIR_ARG/
 
ROOT_DIR=$TEST_SRCDIR/$TEST_WORKSPACE/third_party/raksha/src/ir/auth_logic

diff <(sort $OUTPUT_SOUFFLE_FILE_ARG) <(sort $EXPECTED_DATALOG_FILE)

CMD_RESULT=$?

if [ ${TEST_QUERIES} -eq 1 ]; then
  while read line; do
    arr=(${line//","/ })
    if [ -s $OUTPUT_QUERY_DIR_ARG/${arr[0]} ]; then
      if [ ${arr[1]} == "true" ]; then
        QUERY_TEST_RESULT=1
      else
        echo "query mismatch"
        exit 1;
      fi
    else
      if [ ${arr[1]} == "true" ]; then
        echo "query mismatch"
        exit 1
      else
        QUERY_TEST_RESULT=1
      fi
    fi
      
  done < $EXPECTED_QUERIES_FILE
else
  echo "No need to test queries"
fi

if ([ ${CMD_RESULT} -eq 0 ] && [ ${QUERY_TEST_RESULT} -eq 1 ]); then
  exit 0;
else
  echo "Expected datalog and generated datalog are different!"
  exit 1
fi
