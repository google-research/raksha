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
DP_ANALYSIS="dp_analysis"
SQL_POLICY_ANALYSIS = "sql_policy_analysis"

function printUsageAndExit() {
  echo "Usage: "
  echo "  For sql_policy_rules: "
  echo "    check_policy_compliance_test.sh \ "
  echo "      <cmd> ${PASS_RESULT}|${FAIL_RESULT} \"sql_policy_analysis\" <ir|proto> <file> <sql_policy_rules_file> <policy_engine>"
  echo "  For differential privacy analysis: "
  echo "     <cmd> ${PASS_RESULT}|${FAIL_RESULT} \"dp_analysis\" <ir> <epsilon_parameter> <delta_parameter>"
  exit 1
}

MAX_NUM_ARGS=7
MIN_NUM_ARGS=4

if [[ $# -lt ${MIN_NUM_ARGS} || $# -gt ${MAX_NUM_ARGS} ]]; then
  echo "Found $# arguments, but expected between ${MIN_NUM_ARGS} and ${MAX_NUM_ARGS}."
  printUsageAndExit
fi

CMD_ARG=$1
EXPECTATION_ARG=$2
ANALYSIS_TYPE=$3

if [ "${ANALYSIS_TYPE}" != "${DP_ANALYSIS}" ]; then
  TYPE_ARG=$4
  FILE_ARG=$5
  SQL_POLICY_RULES_FILE_ARG=$6
  POLICY_ENGINE=$7
else
 FILE_ARG=$4
 EPSILON_PARAMETER=$5
 DELTA_PARAMETER=$6
fi

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

if [ "${ANALYSIS_TYPE}" != "${DP_ANALYSIS}" ]; then
  SQL_POLICY_RULES_FILE=${ROOT_DIR}/${SQL_POLICY_RULES_FILE_ARG}
  $CMD --$TYPE_ARG=$FILE --sql_policy_rules=$SQL_POLICY_RULES_FILE --policy_engine=$POLICY_ENGINE
else
  $CMD --ir $FILE --epsilon_dp_parameter $EPSILON_PARAMETER --delta_dp_parameter $DELTA_PARAMETER
fi

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
