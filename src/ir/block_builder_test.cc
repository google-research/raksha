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
#include "src/ir/block_builder.h"

#include <memory>

#include "src/common/testing/gtest.h"
#include "src/ir/attribute.h"
#include "src/ir/ir_context.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/type.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {
namespace {

class BlockBuilderTest : public testing::Test {
 public:
  BlockBuilderTest() {
    context_.RegisterOperator(std::make_unique<Operator>("core.plus"));
    context_.RegisterOperator(std::make_unique<Operator>("core.minus"));
    context_.RegisterOperator(std::make_unique<Operator>("core.merge"));
    context_.RegisterOperator(std::make_unique<Operator>("core.choose"));
  }

  types::Type MakeTestEntityType(absl::string_view schema_name) {
    const types::Schema& schema =
        type_factory_.RegisterSchema(std::string(schema_name), {});
    return type_factory_.MakeEntityType(schema);
  }

  types::Type MakePrimitiveType() { return type_factory_.MakePrimitiveType(); }

 protected:
  types::TypeFactory type_factory_;
  IRContext context_;
};

using BlockBuilderDeathTest = BlockBuilderTest;

TEST_F(BlockBuilderTest, DefaultConstructorInitilizesModuleToNullptr) {
  Block block;
  EXPECT_EQ(block.module(), nullptr);
}

void CheckExpectedDecls(const DataDeclCollection& decls,
                        absl::string_view entity_key,
                        absl::string_view primitive_key,
                        absl::string_view schema_name) {
  // TODO(#279): This will be simplified when types are canonicalized
  // and we have a nicer comparator.
  const DataDecl* lhs_decl = decls.FindDecl(entity_key);
  ASSERT_NE(lhs_decl, nullptr);
  const types::TypeBase& lhs_type_base = lhs_decl->type().type_base();
  ASSERT_EQ(lhs_type_base.kind(), types::TypeBase::Kind::kEntity);
  EXPECT_EQ(
      static_cast<const types::EntityType&>(lhs_type_base).schema().name(),
      schema_name);
  const DataDecl* rhs_decl = decls.FindDecl(primitive_key);
  ASSERT_NE(rhs_decl, nullptr);
  const types::TypeBase& rhs_type_base = rhs_decl->type().type_base();
  ASSERT_EQ(rhs_type_base.kind(), types::TypeBase::Kind::kPrimitive);
}

TEST_F(BlockBuilderTest, AddInputUpdatesInputDecls) {
  BlockBuilder builder;
  builder.AddInput("lhs", MakeTestEntityType("InputTensor"));
  builder.AddInput("rhs", MakePrimitiveType());
  auto block = builder.build();
  CheckExpectedDecls(block->inputs(), /*entity_key=*/"lhs",
                     /*primitive_key=*/"rhs",
                     /*schema_name=*/"InputTensor");
}

TEST_F(BlockBuilderTest, AddOutputUpdatesOutputDecls) {
  BlockBuilder builder;
  builder.AddOutput("entity", MakeTestEntityType("OutputTensor"));
  builder.AddOutput("primitive", MakePrimitiveType());
  auto block = builder.build();
  CheckExpectedDecls(block->outputs(), /*entity_key=*/"entity",
                     /*primitive_key=*/"primitive",
                     /*schema_name=*/"OutputTensor");
}

TEST_F(BlockBuilderTest, AddResultsUpdatesResults) {
  BlockBuilder builder;
  auto some_storage =
      std::make_unique<Storage>("secret_store", MakePrimitiveType());
  builder.AddOutput("entity", MakeTestEntityType("Tensor"));
  builder.AddOutput("primitive", MakePrimitiveType());
  builder.AddResult("entity", Value(value::Any()));
  builder.AddResult("primitive", Value(value::StoredValue(*some_storage)));
  auto block = builder.build();
  const NamedValueMap& results = block->results();
  // TODO(#337): A comparator for values will avoid the need to use
  // `ToString()`.
  SsaNames ssa_names;
  ASSERT_EQ(results.count("entity"), 1);
  EXPECT_EQ(results.at("entity").ToString(ssa_names), "<<ANY>>");
  ASSERT_EQ(results.count("primitive"), 1);
  EXPECT_EQ(results.at("primitive").ToString(ssa_names),
            "store:secret_store:type");
}

TEST_F(BlockBuilderDeathTest, AddResultsVerifiesOutputIsDeclared) {
  BlockBuilder builder;
  EXPECT_DEATH({ builder.AddResult("result", Value(value::Any())); },
               "Output 'result' is not declared in the block.");
}

TEST_F(BlockBuilderTest, AddOperationUpdatesOperationList) {
  BlockBuilder builder;
  // std::vector<const Operator*> operations;
  const Operation* plus_op = std::addressof(
      builder.AddOperation(context_.GetOperator("core.plus"), {}, {}));
  const Operation* minus_op = std::addressof(
      builder.AddOperation(context_.GetOperator("core.minus"), {}, {}));
  const Operation* merge_op = std::addressof(
      builder.AddOperation(context_.GetOperator("core.merge"), {}, {}));
  auto block = builder.build();
  EXPECT_THAT(block->operations(),
              testing::ElementsAre(testing::Pointer(testing::Eq(plus_op)),
                                   testing::Pointer(testing::Eq(minus_op)),
                                   testing::Pointer(testing::Eq(merge_op))));
}

TEST_F(BlockBuilderTest, AddImplementationPassesSelfAndResultBlock) {
  BlockBuilder builder;
  Block* passed_in_block;
  builder.AddImplementation([&builder, &passed_in_block](
                                BlockBuilder& this_builder, Block& this_block) {
    passed_in_block = std::addressof(this_block);
    EXPECT_EQ(std::addressof(builder), std::addressof(this_builder));
  });

  auto block = builder.build();
  EXPECT_EQ(block.get(), passed_in_block);
}

TEST_F(BlockBuilderTest, AddImplementationMakingMultipleUpdates) {
  BlockBuilder builder;
  builder.AddImplementation([this](BlockBuilder& builder, Block& block) {
    builder.AddInput("primitive_input", MakePrimitiveType());
    builder.AddInput("entity_input", MakeTestEntityType("InputTensor"));
    builder.AddOutput("primitive_output", MakePrimitiveType());
    builder.AddOutput("entity_output", MakeTestEntityType("OutputTensor"));
    const Operation& op =
        builder.AddOperation(context_.GetOperator("core.plus"), {}, {});
    builder.AddResult("primitive_output",
                      Value(value::OperationResult(op, "primitive_value")));
    builder.AddResult("entity_output",
                      Value(value::OperationResult(op, "entity_value")));
  });

  auto block = builder.build();
  CheckExpectedDecls(block->inputs(), /*entity_key=*/"entity_input",
                     /*primitive_key=*/"primitive_input",
                     /*schema_name=*/"InputTensor");
  CheckExpectedDecls(block->outputs(), /*entity_key=*/"entity_output",
                     /*primitive_key=*/"primitive_output",
                     /*schema_name=*/"OutputTensor");
  const NamedValueMap& results = block->results();
  SsaNames ssa_names;
  ASSERT_EQ(results.count("entity_output"), 1);
  EXPECT_EQ(results.at("entity_output").ToString(ssa_names), "%0.entity_value");
  ASSERT_EQ(results.count("primitive_output"), 1);
  EXPECT_EQ(results.at("primitive_output").ToString(ssa_names),
            "%0.primitive_value");
}

}  // namespace
}  // namespace raksha::ir
