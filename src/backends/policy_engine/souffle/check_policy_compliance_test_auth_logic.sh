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
  echo "  check_policy_compliance_test_auth_logic.sh \ "
  echo "     <policy_cmd> <check_compliance_cmd> ${PASS_RESULT}|${FAIL_RESULT} <ir_file> <policy_rules_file>"
  exit 1
}

if [ $# -ne 4 ]; then
  printUsageAndExit
fi

POLICY_CMD_ARG = $1
COMPLIANCE_CMD_ARG=$2
EXPECTATION_ARG=$3
IR_FILE_ARG=$4
POLICY_RULES_FILE_ARG=$5

if
  [ "${EXPECTATION_ARG}" != "${PASS_RESULT}" ] &&
  [ "${EXPECTATION_ARG}" != "${FAIL_RESULT}" ];
then
  printUsageAndExit
fi

# A simple script to test the `check_policy_compliance` command line tool.
ROOT_DIR=${TEST_SRCDIR}/${TEST_WORKSPACE}
COMPLIANCE_CMD=${ROOT_DIR}/${COMPLIANCE_CMD_ARG}
POLICY_CMD = ${ROOT_DIR}/${POLICY_CMD_ARG}
IR_FILE=${ROOT_DIR}/${IR_FILE_ARG}
POLICY_RULES_FILE=${ROOT_DIR}/${POLICY_RULES_FILE_ARG}

$POLICY_CMD --policy_rules=$POLICY_RULES_FILE --datalog_file=$ROOT_DIR/testdata/
POLICY_CMD_RESULT=$?
if [ ${POLICY_CMD_RESULT} -neq 0 ]; then
    echo "Policy command failed"
    exit 1
fi

blaze build ${ROOT_DIR}:check_policy_compliance  # Or Extract from file name
$COMPLIANCE_CMD --ir=$IR_FILE --sql_policy_rules=$POLICY_RULES_FILE

COMPLIANCE_CMD_RESULT=$?
if [ ${COMPLIANCE_CMD_RESULT} -eq 0 ]; then
  ACTUAL_RESULT="${PASS_RESULT}"
elif [ ${COMPLIANCE_CMD_RESULT} -eq 1 ]; then
  ACTUAL_RESULT="${FAIL_RESULT}"
else
  ACTUAL_RESULT="UNKNOWN"
fi

if [ "${ACTUAL_RESULT}" != "${EXPECTATION_ARG}" ]; then
  echo "Policy compliance check does not match expectations!"
  echo "  Expected: ${EXPECTATION_ARG}, Actual: ${ACTUAL_RESULT}"
  exit 1
else
  exit 0
fi
