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
  echo "  auth_logic_prototype.sh \ "
  echo "     <auth_logic_file> <output_souffle_file> <output_query_directory>"
  exit 1
}

CMD_ARG=$1
AUTH_LOGIC_FILE_ARG=$2
#Following args are optional
OUTPUT_SOUFFLE_FILE_ARG=${3:-$(mktemp)}
OUTPUT_QUERY_DIR_ARG=${4:-$(mktemp -d)}
#To-do make below args optional
#TEST_ONLY=${5:-0}

$CMD_ARG  --policy_rules=$AUTH_LOGIC_FILE_ARG  --datalog_file=$OUTPUT_SOUFFLE_FILE_ARG --skip_declarations=true

if [ -z "$SOUFFLE_BIN" ]
then
        echo "Set SOUFFLE_BIN"
        exit 1
else
        echo $SOUFFLE_BIN
fi

PATH=$PATH:third_party/mcpp
$SOUFFLE_BIN $OUTPUT_SOUFFLE_FILE_ARG -D $OUTPUT_QUERY_DIR_ARG/

echo $(cat $OUTPUT_SOUFFLE_FILE_ARG)
echo $(ls $OUTPUT_QUERY_DIR_ARG)

#Compare diff for test TEST_ONLY

