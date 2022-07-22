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
#include "src/ir/attributes/attribute.h"
#include "src/ir/ir_context.h"
#include "src/ir/ir_printer.h"
#include "src/ir/ssa_names.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/type.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {
namespace {

class BlockBuilderTest : public testing::Test {
 public:
  BlockBuilderTest()
      : type_factory_(),
        context_(
            {
                Operator("core.plus"),
                Operator("core.minus"),
                Operator("core.merge"),
                Operator("core.choose"),
            },
            {}) {}

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
  EXPECT_EQ(block.parent_module(), nullptr);
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
  const Operator& core_plus =
      *CHECK_NOTNULL(context_.GetOperator("core.plus"));
  const Operator& core_minus =
      *CHECK_NOTNULL(context_.GetOperator("core.plus"));
  const Operator& core_merge =
      *CHECK_NOTNULL(context_.GetOperator("core.merge"));
  // std::vector<const Operator*> operations;
  const Operation* plus_op =
      std::addressof(builder.AddOperation(core_plus, {}, {}));
  const Operation* minus_op =
      std::addressof(builder.AddOperation(core_minus, {}, {}));
  const Operation* merge_op =
      std::addressof(builder.AddOperation(core_merge, {}, {}));
  const Operation* merge_op_with_module = std::addressof(
      builder.AddOperation(core_merge, {}, {}, std::make_unique<Module>()));

  auto block = builder.build();
  EXPECT_THAT(block->operations(),
              testing::ElementsAre(
                  testing::Pointer(testing::Eq(plus_op)),
                  testing::Pointer(testing::Eq(minus_op)),
                  testing::Pointer(testing::Eq(merge_op)),
                  testing::Pointer(testing::Eq(merge_op_with_module))));
}

TEST_F(BlockBuilderTest, AddOperationOfUniquePtrUpdatesOperationList) {
  BlockBuilder builder;
  const Operator& core_plus =
      *CHECK_NOTNULL(context_.GetOperator("core.plus"));
  const Operator& core_minus =
      *CHECK_NOTNULL(context_.GetOperator("core.minus"));
  const Operator& core_merge =
      *CHECK_NOTNULL(context_.GetOperator("core.merge"));

  const Operation* plus_op =
      std::addressof(builder.AddOperation(std::make_unique<Operation>(
          nullptr, core_plus, NamedAttributeMap(), ValueList(), nullptr)));
  const Operation* minus_op =
      std::addressof(builder.AddOperation(std::make_unique<Operation>(
          nullptr, core_minus, NamedAttributeMap(), ValueList(), nullptr)));
  const Operation* merge_op =
      std::addressof(builder.AddOperation(std::make_unique<Operation>(
          nullptr, core_merge, NamedAttributeMap(), ValueList(), nullptr)));
  const Operation* merge_op_with_module = std::addressof(builder.AddOperation(
      std::make_unique<Operation>(nullptr, core_merge, NamedAttributeMap(),
                                  ValueList(), std::make_unique<Module>())));

  auto block = builder.build();
  EXPECT_THAT(block->operations(),
              testing::ElementsAre(
                  testing::Pointer(testing::Eq(plus_op)),
                  testing::Pointer(testing::Eq(minus_op)),
                  testing::Pointer(testing::Eq(merge_op)),
                  testing::Pointer(testing::Eq(merge_op_with_module))));
}

TEST_F(BlockBuilderTest, SetParentPtrErrorsWithPredefinedParent) {
  BlockBuilder builder;
  auto block = builder.build();
  const Operator& core_merge =
      *CHECK_NOTNULL(context_.GetOperator("core.merge"));
  EXPECT_DEATH(builder.AddOperation(std::make_unique<Operation>(
                   block.get(), core_merge, NamedAttributeMap(), ValueList(),
                   std::make_unique<Module>())),
               "Parent pointer already defined. Cannot set parent again!");
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

TEST_F(BlockBuilderTest, GetBlockPtrReturnsCorrectBlockPtr) {
  // Ensures that building a block doesn't move the block.
  // This is important to maintain pointers to blocks that are still being
  // constructed.
  BlockBuilder builder;
  Block* block_ptr = builder.GetBlockPtr();
  auto block = builder.build();
  EXPECT_EQ(block_ptr, block.get());
}

TEST_F(BlockBuilderTest, AddImplementationDoesNotChangeBlockAddress) {
  BlockBuilder builder;
  Block* passed_in_block;
  Block* block_ptr = builder.GetBlockPtr();
  builder.AddImplementation([&builder, &passed_in_block](
                                BlockBuilder& this_builder, Block& this_block) {
    passed_in_block = std::addressof(this_block);
    EXPECT_EQ(std::addressof(builder), std::addressof(this_builder));
  });

  EXPECT_EQ(block_ptr, passed_in_block);
  auto block = builder.build();
  EXPECT_EQ(block.get(), passed_in_block);
  EXPECT_EQ(block_ptr, block.get());
}

TEST_F(BlockBuilderTest, AddOperationDoesNotChangeBlockAddress) {
  BlockBuilder builder;
  Block* block_ptr = builder.GetBlockPtr();
  const Operator& core_plus = *CHECK_NOTNULL(context_.GetOperator("core.plus"));
  const Operation& op = builder.AddOperation(core_plus, {}, {});
  auto block = builder.build();
  EXPECT_THAT(block->operations(),
              testing::ElementsAre(testing::Pointer(testing::Eq(&op))));
  EXPECT_EQ(block_ptr, block.get());
}

TEST_F(BlockBuilderTest, AddImplementationMakingMultipleUpdates) {
  BlockBuilder builder;
  SsaNames ssa_names;
  builder.AddImplementation(
      [this, &ssa_names](BlockBuilder& builder, Block& block) {
        builder.AddInput("primitive_input", MakePrimitiveType());
        builder.AddInput("entity_input", MakeTestEntityType("InputTensor"));
        builder.AddOutput("primitive_output", MakePrimitiveType());
        builder.AddOutput("entity_output", MakeTestEntityType("OutputTensor"));
        const Operator& core_plus =
            *CHECK_NOTNULL(context_.GetOperator("core.plus"));
        const Operation& op = builder.AddOperation(core_plus, {}, {});
        builder.AddResult("primitive_output",
                          Value(value::OperationResult(op, "primitive_value")));
        ssa_names.AddID(Value(value::OperationResult(op, "primitive_value")),
                        "primitive_value");
        builder.AddResult("entity_output",
                          Value(value::OperationResult(op, "entity_value")));
        ssa_names.AddID(Value(value::OperationResult(op, "entity_value")),
                        "entity_value");
      });

  auto block = builder.build();
  CheckExpectedDecls(block->inputs(), /*entity_key=*/"entity_input",
                     /*primitive_key=*/"primitive_input",
                     /*schema_name=*/"InputTensor");
  CheckExpectedDecls(block->outputs(), /*entity_key=*/"entity_output",
                     /*primitive_key=*/"primitive_output",
                     /*schema_name=*/"OutputTensor");
  const NamedValueMap& results = block->results();
  ASSERT_EQ(results.count("entity_output"), 1);
  //%0 is the entity_value OperationResult Value :
  // Value(value::OperationResult(op, "entity_value"))
  EXPECT_EQ(results.at("entity_output").ToString(ssa_names), "entity_value");
  ASSERT_EQ(results.count("primitive_output"), 1);
  //%0 is the primitive_value OperationResult Value :
  // Value(value::OperationResult(op, "primitive_value"))
  EXPECT_EQ(results.at("primitive_output").ToString(ssa_names),
            "primitive_value");
}

// A class to test `AddOperation`  with custom create methods.
class TestOperation : public Operation {
 public:
  static std::unique_ptr<Operation> Create(const Block* parent_block,
                                           IRContext& context,
                                           absl::string_view op_name) {
    if (!context.IsRegisteredOperator(op_name)) return nullptr;
    // `parent_block` is  nullptr if op_name is `core.merge` for death tests.
    return std::make_unique<Operation>(
        op_name == "core.merge" ? nullptr : parent_block,
        *CHECK_NOTNULL(context.GetOperator(op_name)), NamedAttributeMap({}),
        ValueList({}));
  }
};

TEST_F(BlockBuilderTest, AllowsAdditionOfPreConstructedOperation) {
  BlockBuilder builder;
  const Operator& core_plus = *CHECK_NOTNULL(context_.GetOperator("core.plus"));
  std::unique_ptr<Operation> plus_op_ptr =
      std::make_unique<Operation>(core_plus);
  const Operation* plus_op =
      std::addressof(builder.AddOperation(std::move(plus_op_ptr)));

  const Operator& core_minus =
      *CHECK_NOTNULL(context_.GetOperator("core.minus"));
  std::unique_ptr<Operation> minus_op_ptr =
      std::make_unique<Operation>(core_minus);
  const Operation* minus_op =
      std::addressof(builder.AddOperation(std::move(minus_op_ptr)));

  auto block = builder.build();
  EXPECT_THAT(block->operations(),
              testing::ElementsAre(testing::Pointer(testing::Eq(plus_op)),
                                   testing::Pointer(testing::Eq(minus_op))));
}

TEST_F(BlockBuilderTest, AllowsAdditionOfOperationWithCustomCreateMethod) {
  BlockBuilder builder;
  const Operation* plus_op = std::addressof(
      builder.AddOperation<TestOperation>(context_, "core.plus"));
  const Operation* minus_op = std::addressof(
      builder.AddOperation<TestOperation>(context_, "core.minus"));
  auto block = builder.build();
  EXPECT_THAT(block->operations(),
              testing::ElementsAre(testing::Pointer(testing::Eq(plus_op)),
                                   testing::Pointer(testing::Eq(minus_op))));
}

using BlockBuilderDeathTest = BlockBuilderTest;

TEST_F(BlockBuilderDeathTest, AddOperationFailsIfCustomCreateReturnsNullptr) {
  BlockBuilder builder;
  EXPECT_DEATH(
      {
        builder.AddOperation<TestOperation>(context_, "some.unknown.operator");
      },
      "`T::Create` returned a nullptr when adding an operation.");
}

TEST_F(BlockBuilderDeathTest, AddOperationFailsIfCustomCreateSetsWrongBlock) {
  BlockBuilder builder;
  EXPECT_DEATH({ builder.AddOperation<TestOperation>(context_, "core.merge"); },
               "`T::Create` did not set the correct parent block.");
}

TEST_F(BlockBuilderDeathTest, InvokingBuildMoreThanOnceFails) {
  BlockBuilder builder;
  std::unique_ptr<Block> block1 = builder.build();
  EXPECT_DEATH({ builder.build(); },
               "Attempt to build a `BlockBuilder` that has already been"
               " built.");
}

TEST_F(BlockBuilderDeathTest, InvokingGetBlockPtrAfterBuildFails) {
  BlockBuilder builder;
  std::unique_ptr<Block> block1 = builder.build();
  EXPECT_DEATH(
      { builder.GetBlockPtr(); },
      "Attempt to get block pointer from a `BlockBuilder` that has already been"
      " built.");
}

}  // namespace
}  // namespace raksha::ir
