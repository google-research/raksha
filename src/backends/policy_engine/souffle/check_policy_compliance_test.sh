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

PASS_RESULT="pass"
FAIL_RESULT="fail"

function printUsageAndExit() {
  echo "Usage: "
  echo "  check_policy_compliance_test.sh \ "
  echo "     <cmd> ${PASS_RESULT}|${FAIL_RESULT} <ir|proto> <file> <sql_policy_rules_file> <policy_engine>"
  exit 1
}

MAX_NUM_ARGS=6
MIN_NUM_ARGS=5

if [ $# -ne ${MAX_NUM_ARGS} || $# -ne ${MIN_NUM_ARGS}]; then
  printUsageAndExit
fi

CMD_ARG=$1
EXPECTATION_ARG=$2
TYPE_ARG=$3
FILE_ARG=$4
SQL_POLICY_RULES_FILE_ARG=$5
POLICY_ENGINE=$6

if
  [ "${EXPECTATION_ARG}" != "${PASS_RESULT}" ] &&
  [ "${EXPECTATION_ARG}" != "${FAIL_RESULT}" ];
then
  printUsageAndExit
fi
# A simple script to test the `check_policy_compliance` command line tool.
ROOT_DIR=${TEST_SRCDIR}/${TEST_WORKSPACE}
CMD=${ROOT_DIR}/${CMD_ARG}
FILE=${ROOT_DIR}/${FILE_ARG}
SQL_POLICY_RULES_FILE=${ROOT_DIR}/${SQL_POLICY_RULES_FILE_ARG}

$CMD --$TYPE_ARG=$FILE --sql_policy_rules=$SQL_POLICY_RULES_FILE --policy_engine=$POLICY_ENGINE
CMD_RESULT=$?
if [ ${CMD_RESULT} -eq 0 ]; then
  ACTUAL_RESULT="${PASS_RESULT}"
elif [ ${CMD_RESULT} -eq 1 ]; then
  ACTUAL_RESULT="${FAIL_RESULT}"
else
  ACTUAL_RESULT="UNKNOWN"
fi

if [ "${ACTUAL_RESULT}" != "${EXPECTATION_ARG}" ]; then
  echo "Policy compliance check does not match expectations!"
  echo "  Expected: ${EXPECTATION_ARG}, Actual: ${ACTUAL_RESULT}"
  exit 1;
else
  exit 0
fi
