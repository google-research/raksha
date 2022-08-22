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


CMD_ARG=$1
SQL_POLICY_RULES_FILE_ARG=$2

# A simple script to test the `check_policy_compliance` command line tool.
ROOT_DIR=${TEST_SRCDIR}/${TEST_WORKSPACE}
CMD=${ROOT_DIR}/${CMD_ARG}
SQL_POLICY_RULES_FILE=${ROOT_DIR}/${SQL_POLICY_RULES_FILE_ARG}
ERROR_EXIT_CODE=2

$CMD --ir=$FILE --proto=$FILE --sql_policy_rules=$SQL_POLICY_RULES_FILE
CMD_RESULT=$?
if [ ${CMD_RESULT} -eq ${ERROR_EXIT_CODE} ]; then
  exit 0
else
  exit 1
fi
