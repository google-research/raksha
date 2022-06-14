#!/bin/bash

PASS_RESULT="pass"
FAIL_RESULT="fail"

function printUsageAndExit() {
  echo "Usage: "
  echo "  check_policy_compliance_test.sh \ "
  echo "     ${PASS_RESULT}|${FAIL_RESULT} <ir_file> <sql_policy_rules_file>"
  exit 1
}

if [ $# -ne 3 ]; then
  printUsageAndExit
fi

EXPECTATION_ARG=$1
IR_FILE_ARG=$2
SQL_POLICY_RULES_FILE_ARG=$3

if 
  [ "${EXPECTATION_ARG}" != "${PASS_RESULT}" ] &&
  [ "${EXPECTATION_ARG}" != "${FAIL_RESULT}" ];
then
  printUsageAndExit
fi

echo "IR File arg is ${IR_FILE_ARG}"
echo "SQL Policies R File arg is ${SQL_POLICY_RULES_FILE_ARG}"

# A simple script to test the `check_policy_compliance` command line tool.
ROOT_DIR=${TEST_SRCDIR}/${TEST_WORKSPACE}
CMD=${ROOT_DIR}/src/backends/policy_engine/souffle/check_policy_compliance
IR_FILE=${ROOT_DIR}/${IR_FILE_ARG}
SQL_POLICY_RULES_FILE=${ROOT_DIR}/${SQL_POLICY_RULES_FILE_ARG}

$CMD --ir=$IR_FILE --sql_policy_rules=$SQL_POLICY_RULES_FILE
if [ $? -eq 0 ]; then
  ACTUAL_RESULT="${PASS_RESULT}"
else 
  ACTUAL_RESULT="${FAIL_RESULT}"
fi
if [ "${ACTUAL_RESULT}" != ${EXPECTATION_ARG} ]; then
  echo "Policy compliance check does not match expectations!"
  echo "  Expected: ${EXPECTATION_ARG}, Actual: ${ACTUAL_RESULT}"
  exit 1;
else
  exit 0
fi
