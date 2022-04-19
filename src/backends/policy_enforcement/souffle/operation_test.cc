//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include "src/backends/policy_enforcement/souffle/operation.h"

#include "absl/strings/string_view.h"
#include "src/backends/policy_enforcement/souffle/fact.h"
#include "src/backends/policy_enforcement/souffle/value.h"
#include "src/common/testing/gtest.h"

namespace raksha::backends::policy_enforcement::souffle {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

struct IsOperationFactAndExpectedDatalog {
  const IsOperationFact *is_operation_fact;
  absl::string_view expected_datalog;
};

class IsOperationFactTest
    : public TestWithParam<IsOperationFactAndExpectedDatalog> {};

TEST_P(IsOperationFactTest, IsOperationFactTest) {
  auto [is_operation_fact, expected_datalog] = GetParam();
  EXPECT_EQ(is_operation_fact->ToDatalogString(), expected_datalog);
}

static const IsOperationFact kSampleFact1 = IsOperationFact(Operation(
    Symbol("UserA"), Symbol("sql.Literal"), Symbol("out"), OperandList(),
    AttributeList(
        Attribute(Symbol("literal_value"),
                  AttributePayload(StringAttributePayload(Symbol("number_5")))),
        AttributeList())));

static const IsOperationFact kSampleFact2 = IsOperationFact(Operation(
    Symbol("UserB"), Symbol("sql.MergeOp"), Symbol("out"),
    OperandList(Symbol("input1"), OperandList(Symbol("input2"), OperandList())),
    AttributeList()));

static IsOperationFactAndExpectedDatalog kSampleIsOperationFactsAndDatalog[] = {
    {.is_operation_fact = &kSampleFact1,
     .expected_datalog =
         R"(isOperation(["UserA", "sql.Literal", "out", nil, [["literal_value", $StringAttributePayload{"number_5"}], nil]]).)"},
    {.is_operation_fact = &kSampleFact2,
     .expected_datalog =
         R"(isOperation(["UserB", "sql.MergeOp", "out", ["input1", ["input2", nil]], nil]).)"}};

INSTANTIATE_TEST_SUITE_P(IsOperationFactTest, IsOperationFactTest,
                         ValuesIn(kSampleIsOperationFactsAndDatalog));

}  // namespace raksha::backends::policy_enforcement::souffle
