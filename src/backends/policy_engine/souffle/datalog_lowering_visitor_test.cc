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
#include "src/ir/attributes/float_attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/datalog/operation.h"
#include "src/ir/datalog/value.h"
#include "src/ir/module.h"

namespace raksha::backends::policy_engine::souffle {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

class TestObjectFactory {
 public:
  // Helper to create operation for tests.
  template <typename... Args>
  const ir::Operation *CreateOperation(Args &&...a) {
    operations_.push_back(
        std::make_unique<ir::Operation>(std::forward<Args>(a)...));
    return operations_.back().get();
  }

  // Creates and returns an IsOperationFact.
  template <typename... Args>
  const ir::datalog::IsOperationFact *CreateDatalogOperation(Args &&...a) {
    datalog_operations_.push_back(
        std::make_unique<ir::datalog::IsOperationFact>(
            ir::datalog::Operation(std::forward<Args>(a)...)));
    return datalog_operations_.back().get();
  }

 private:
  ir::Block::OperationList operations_;
  std::vector<std::unique_ptr<ir::datalog::IsOperationFact>>
      datalog_operations_;
};

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

  DatalogLoweringVisitor datalog_lowering_visitor;
  ir_operation->Accept(datalog_lowering_visitor);
  const RakshaDatalogFacts &facts = datalog_lowering_visitor.datalog_facts();
  EXPECT_EQ(facts.ToDatalogString(), datalog_is_operation->ToDatalogString());
}

static const ir::Operator kLiteralOperator("sql.ReadLiteral");
static const ir::Operator kMergeOpOperator("sql.MergeOp");

static TestObjectFactory test_factory;
static const IrAndDatalogOperationPairs kIrAndDatalogOperations[] = {
    {.ir_operation = test_factory.CreateOperation(
         nullptr, kLiteralOperator,
         ir::NamedAttributeMap({
             {"float", ir::Attribute::Create<ir::FloatAttribute>(2.1)},
             {"literal",
              ir::Attribute::Create<ir::StringAttribute>("number_5")},
             {"number", ir::Attribute::Create<ir::Int64Attribute>(3)},
         }),
         ir::ValueList()),
     .datalog_is_operation = test_factory.CreateDatalogOperation(
         ir::datalog::Symbol("sql"), ir::datalog::Symbol("sql.ReadLiteral"),
         ir::datalog::ResultList(ir::datalog::Symbol("%0"),
                                               ir::datalog::ResultList()), ir::datalog::OperandList(),
         ir::datalog::AttributeList(
             ir::datalog::Attribute("float",
                                    ir::datalog::Attribute::Float(2.1)),
             ir::datalog::AttributeList(
                 ir::datalog::Attribute(
                     "literal", ir::datalog::Attribute::String("number_5")),
                 ir::datalog::AttributeList(
                     ir::datalog::Attribute("number",
                                            ir::datalog::Attribute::Number(3)),
                     ir::datalog::AttributeList()))))},
    {.ir_operation = test_factory.CreateOperation(
         nullptr, kMergeOpOperator, ir::NamedAttributeMap({}),
         ir::ValueList(
             {ir::Value(ir::value::Any()), ir::Value(ir::value::Any())})),
     .datalog_is_operation = test_factory.CreateDatalogOperation(
         ir::datalog::Symbol("sql"), ir::datalog::Symbol("sql.MergeOp"),
         ir::datalog::ResultList(ir::datalog::Symbol("%0"),
                                 ir::datalog::ResultList()),
         ir::datalog::OperandList(
             ir::datalog::Symbol("<<ANY>>"),
             ir::datalog::OperandList(ir::datalog::Symbol("<<ANY>>"),
                                      ir::datalog::OperandList())),
         ir::datalog::AttributeList())}};

INSTANTIATE_TEST_SUITE_P(IrOperationLoweringTest, IrOperationLoweringTest,
                         ValuesIn(kIrAndDatalogOperations));

struct ModuleAndExpectedRakshaDatalogFactBuilders {
  std::function<ir::Module()> module_fn;
  std::function<RakshaDatalogFacts()> facts_fn;
};

class DatalogLoweringVisitModuleTest
    : public TestWithParam<ModuleAndExpectedRakshaDatalogFactBuilders> {};

TEST_P(DatalogLoweringVisitModuleTest, DatalogLoweringVisitModuleTest) {
  const auto [module_fn, facts_fn] = GetParam();
  ir::Module module = module_fn();
  RakshaDatalogFacts raksha_datalog_facts = facts_fn();
  DatalogLoweringVisitor visitor;
  module.Accept(visitor);
  EXPECT_EQ(visitor.datalog_facts().ToDatalogString(),
            raksha_datalog_facts.ToDatalogString());
}

static std::function<ir::Module()> ModuleWithAddedOperationsFn(
    std::function<void(ir::BlockBuilder &)> operation_adding_fn) {
  return [=]() {
    ir::BlockBuilder block_builder;
    operation_adding_fn(block_builder);
    ir::Module module;
    module.AddBlock(block_builder.build());
    return module;
  };
}

static const ModuleAndExpectedRakshaDatalogFactBuilders
    kModuleAndExpectedRakshaDatalogFacts[] = {
        // Empty module test.
        {.module_fn = []() { return ir::Module(); },
         .facts_fn = []() { return RakshaDatalogFacts(); }},
        // Test containing a single passthrough operation.
        {.module_fn =
             ModuleWithAddedOperationsFn([](ir::BlockBuilder &builder) {
               builder.AddOperation(kMergeOpOperator, {},
                                    {ir::Value(ir::value::Any())});
             }),
         .facts_fn =
             []() {
               RakshaDatalogFacts facts;
               facts.AddIsOperationFact(
                   ir::datalog::IsOperationFact(ir::datalog::Operation(
                       ir::datalog::Symbol("sql"),
                       ir::datalog::Symbol("sql.MergeOp"),
                       ir::datalog::ResultList(ir::datalog::Symbol("%0"),
                                               ir::datalog::ResultList()),
                       ir::datalog::OperandList(ir::datalog::Symbol("<<ANY>>"),
                                                ir::datalog::OperandList()),
                       ir::datalog::AttributeList())));
               return facts;
             }},
        // Show two passthroughs chained together.
        {.module_fn =
             ModuleWithAddedOperationsFn([](ir::BlockBuilder &builder) {
               const ir::Operation &operation = builder.AddOperation(
                   kMergeOpOperator, {}, {ir::Value(ir::value::Any())});
               builder.AddOperation(
                   kMergeOpOperator, {},
                   {ir::Value(ir::value::OperationResult(operation, "out"))});
             }),
         .facts_fn =
             []() {
               RakshaDatalogFacts facts;
               facts.AddIsOperationFact(
                   ir::datalog::IsOperationFact(ir::datalog::Operation(
                       ir::datalog::Symbol("sql"),
                       ir::datalog::Symbol("sql.MergeOp"),
                       ir::datalog::ResultList(ir::datalog::Symbol("%0"),
                                               ir::datalog::ResultList()),
                       ir::datalog::OperandList(ir::datalog::Symbol("<<ANY>>"),
                                                ir::datalog::OperandList()),
                       ir::datalog::AttributeList())));
               facts.AddIsOperationFact(
                   ir::datalog::IsOperationFact(ir::datalog::Operation(
                       ir::datalog::Symbol("sql"),
                       ir::datalog::Symbol("sql.MergeOp"),
                       ir::datalog::ResultList(ir::datalog::Symbol("%1"),
                                               ir::datalog::ResultList()),
                       ir::datalog::OperandList(ir::datalog::Symbol("%0"),
                                                ir::datalog::OperandList()),
                       ir::datalog::AttributeList())));
               return facts;
             }},
        // Show a diamond of operations.
        {.module_fn = ModuleWithAddedOperationsFn([](ir::BlockBuilder
                                                         &builder) {
           const ir::Operation &start_operation = builder.AddOperation(
               kMergeOpOperator, {}, {ir::Value(ir::value::Any())});
           const ir::Operation &left_operation = builder.AddOperation(
               kMergeOpOperator, {},
               {ir::Value(ir::value::OperationResult(start_operation, "out"))});
           const ir::Operation &right_operation = builder.AddOperation(
               kMergeOpOperator, {},
               {ir::Value(ir::value::OperationResult(start_operation, "out"))});
           builder.AddOperation(
               kMergeOpOperator, {},
               {ir::Value(ir::value::OperationResult(left_operation, "out")),
                ir::Value(ir::value::OperationResult(right_operation, "out"))});
         }),
         .facts_fn = []() {
           RakshaDatalogFacts facts;
           facts.AddIsOperationFact(
               ir::datalog::IsOperationFact(ir::datalog::Operation(
                   ir::datalog::Symbol("sql"),
                   ir::datalog::Symbol("sql.MergeOp"),
                   ir::datalog::ResultList(ir::datalog::Symbol("%0"),
                                           ir::datalog::ResultList()),
                   ir::datalog::OperandList(ir::datalog::Symbol("<<ANY>>"),
                                            ir::datalog::OperandList()),
                   ir::datalog::AttributeList())));
           facts.AddIsOperationFact(
               ir::datalog::IsOperationFact(ir::datalog::Operation(
                   ir::datalog::Symbol("sql"),
                   ir::datalog::Symbol("sql.MergeOp"),
                   ir::datalog::ResultList(ir::datalog::Symbol("%1"),
                                           ir::datalog::ResultList()),
                   ir::datalog::OperandList(ir::datalog::Symbol("%0"),
                                            ir::datalog::OperandList()),
                   ir::datalog::AttributeList())));
           facts.AddIsOperationFact(
               ir::datalog::IsOperationFact(ir::datalog::Operation(
                   ir::datalog::Symbol("sql"),
                   ir::datalog::Symbol("sql.MergeOp"),
                   ir::datalog::ResultList(ir::datalog::Symbol("%2"),
                                           ir::datalog::ResultList()),
                   ir::datalog::OperandList(ir::datalog::Symbol("%0"),
                                            ir::datalog::OperandList()),
                   ir::datalog::AttributeList())));
           facts.AddIsOperationFact(
               ir::datalog::IsOperationFact(ir::datalog::Operation(
                   ir::datalog::Symbol("sql"),
                   ir::datalog::Symbol("sql.MergeOp"),
                   ir::datalog::ResultList(ir::datalog::Symbol("%3"),
                                           ir::datalog::ResultList()),
                   ir::datalog::OperandList(
                       ir::datalog::Symbol("%1"),
                       ir::datalog::OperandList(ir::datalog::Symbol("%2"),
                                                ir::datalog::OperandList())),
                   ir::datalog::AttributeList())));
           return facts;
         }}};

INSTANTIATE_TEST_SUITE_P(DatalogLoweringVisitModuleTest,
                         DatalogLoweringVisitModuleTest,
                         ValuesIn(kModuleAndExpectedRakshaDatalogFacts));

}  // namespace raksha::backends::policy_engine::souffle
