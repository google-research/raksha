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

#include "datalog_lowering_visitor.h"

#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/ir/datalog/value.h"
#include "src/ir/module.h"

namespace raksha::backends::policy_engine::souffle {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

struct IrAndDatalogOperationPairs {
  const ir::Operation *ir_operation;
  const ir::datalog::IsOperationFact *datalog_is_operation;
};

class IrOperationLoweringTest
    : public TestWithParam<IrAndDatalogOperationPairs> {};

// Check that the IrOperation is lowered to the expected `IsOperationFact`.
// Rather than introducing a `operator==` for this, we just rely on them
// producing the same `ToDatalogString` result.
TEST_P(IrOperationLoweringTest, IrOperationLoweringTest) {
  const auto [ir_operation, datalog_is_operation] = GetParam();

  ir::SsaNames ssa_names;
  DatalogLoweringVisitor datalog_lowering_visitor(ssa_names);
  ir_operation->Accept(datalog_lowering_visitor);
  const RakshaDatalogFacts &facts = datalog_lowering_visitor.datalog_facts();
  EXPECT_EQ(facts.ToDatalogString(), datalog_is_operation->ToDatalogString());
}

static const ir::Operator kLiteralOperator("sql.ReadLiteral");

static const ir::Operation kStringLiteralIrOperation(
    nullptr, kLiteralOperator,
    {{"literal", ir::Attribute::Create<ir::StringAttribute>("number_5")}}, {});

static const ir::datalog::IsOperationFact kStringLiteralDatalogIsOperation(
    ir::datalog::Operation(
        ir::datalog::Symbol("UNKNOWN"), ir::datalog::Symbol("sql.ReadLiteral"),
        ir::datalog::Symbol("%0.out"), ir::datalog::OperandList(),
        ir::datalog::AttributeList(
            ir::datalog::Attribute(ir::datalog::Symbol("literal"),
                                   ir::datalog::StringAttributePayload(
                                       ir::datalog::Symbol("number_5"))),
            ir::datalog::AttributeList())));

static const ir::Operator kMergeOpOperator("sql.MergeOp");

static const ir::Operation kMergeOpIrOperation(nullptr, kMergeOpOperator, {},
                                               {ir::Value(ir::value::Any()),
                                                ir::Value(ir::value::Any())});

static const ir::datalog::IsOperationFact kMergeOpIsOperation(
    ir::datalog::Operation(
        ir::datalog::Symbol("UNKNOWN"), ir::datalog::Symbol("sql.MergeOp"),
        ir::datalog::Symbol("%0.out"),
        ir::datalog::OperandList(
            ir::datalog::Symbol("<<ANY>>"),
            ir::datalog::OperandList(ir::datalog::Symbol("<<ANY>>"),
                                     ir::datalog::OperandList())),
        ir::datalog::AttributeList()));

static const IrAndDatalogOperationPairs kIrAndDatalogOperations[] = {
    {.ir_operation = &kStringLiteralIrOperation,
     .datalog_is_operation = &kStringLiteralDatalogIsOperation},
    {.ir_operation = &kMergeOpIrOperation,
     .datalog_is_operation = &kMergeOpIsOperation}};

INSTANTIATE_TEST_SUITE_P(IrOperationLoweringTest, IrOperationLoweringTest,
                         ValuesIn(kIrAndDatalogOperations));

struct ModuleBuildingFnAndExpectedRakshaDatalogFacts {
  std::function<ir::Module()> module_building_fn;
  intrusive_ptr<RakshaDatalogFacts> expected_raksha_datalog_facts;
};

class DatalogLoweringVisitModuleTest : public
    TestWithParam<ModuleBuildingFnAndExpectedRakshaDatalogFacts> {};

TEST_P(DatalogLoweringVisitOperationTest, DatalogLoweringVisitOperationTest) {
  const auto &[module, facts] = GetParam();
  ir::SsaNames ssa_names;
  DatalogLoweringVisitor visitor(ssa_names);
  module.Accept(visitor);
  EXPECT_EQ(visitor.datalog_facts().ToDatalogString(), facts.ToDatalogString());
}

static const ModuleAndExpectedRakshaDatalogFacts
kModuleAndExpectedRakshaDatalogFacts[] = {
    { .module = }
};

}  // namespace raksha::backends::policy_engine::souffle
