#include "absl/container/flat_hash_set.h"
#include "src/common/testing/gtest.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ssa_names.h"
#include "src/ir/storage.h"
#include "src/ir/types/primitive_type.h"
#include "src/ir/value.h"

namespace raksha::ir {

TEST(NewIrTest, TrySomeStuff) {
  IRContext context;
  types::TypeFactory& type_factory = context.type_factory();

  // Using PrimitiveType for illustrative purposes.
  // In reality we will have to create the correct types.
  // For generic operators, we should also create a TypeVariable type.

  const Operator& constant_op =
      context.RegisterOperator(std::make_unique<Operator>("core.constant"));
  // ,
  //           BlockBuilder()
  //               .AddOutput("result", type_factory.MakePrimitiveType())
  //               .build())  ASSERT_NE(constant_op, nullptr);
  //   const Operator& select_op =
  //       context.RegisterOperator(std::make_unique<Operator>("sql.select"));
  //   ,
  //   BlockBuilder()
  //       .AddInput("input", type_factory.MakePrimitiveType())
  //       .AddOutput("output", type_factory.MakePrimitiveType())
  //       .build())
  // ASSERT_NE(select_op, nullptr);

  const Operator& tag_claim_op =
      context.RegisterOperator(std::make_unique<Operator>("arcs.tag_claim"));
  //   ,
  //   BlockBuilder()
  //       .AddInput("input", type_factory.MakePrimitiveType())
  //       .AddInput("predicate", type_factory.MakePrimitiveType())
  //       .AddOutput("output", type_factory.MakePrimitiveType())
  //       .build()
  // ASSERT_NE(tag_claim_op, nullptr);

  // Foo {a, b}
  const Operator& make_foo_op =
      context.RegisterOperator(std::make_unique<Operator>("core.makeFoo"));
  //   ,
  //   BlockBuilder()
  //       .AddInput("a", type_factory.MakePrimitiveType())
  //       .AddInput("b", type_factory.MakePrimitiveType())
  //       .AddOutput("value", type_factory.MakePrimitiveType())
  //       .build())
  // ASSERT_NE(make_foo_op, nullptr);

  //   const Operator& make_particle_op =
  //       context.RegisterOperator(std::make_unique<Operator>(
  //           "core.make_particle"));  // , BlockBuilder().build()
  // ASSERT_NE(make_particle_op, nullptr);

  //   const Operator& call_particle_op =
  //       context.RegisterOperator(std::make_unique<Operator>(
  //           "core.call_particle"));  // , BlockBuilder().build()
  // ASSERT_NE(call_particle_op, nullptr);

  const Operator& read_field_op =
      context.RegisterOperator(std::make_unique<Operator>(
          "core.read_field"));  // , BlockBuilder().build()
                                // ASSERT_NE(read_field_op, nullptr);

  //   const Operator& choose_op = context.RegisterOperator(
  //       std::make_unique<Operator>("core.choose"));  // ,
  //       BlockBuilder().build()
  // ASSERT_NE(choose_op, nullptr);

  const Operator& merge_op =
      context.RegisterOperator(std::make_unique<Operator>("core.merge"));  // ,
  //       BlockBuilder().build()
  // ASSERT_NE(merge_op, nullptr);

  // Write Storage
  const Operator& write_op =
      context.RegisterOperator(std::make_unique<Operator>("core.write"));
  //   , BlockBuilder()
  //                     .AddInput("src", type_factory.MakePrimitiveType())
  //                     .AddInput("tgt", type_factory.MakePrimitiveType())
  //                     .build()
  // ASSERT_NE(make_foo_op, nullptr);

  // particle P1 as an operator
  //
  // Foo {a, b}
  // Bar {x, y}
  //
  // particle P1
  //  .bar: reads Bar {}
  //   foo: writes Foo {}
  //   claim foo.a is userSelection
  //   claim foo.b is derived from bar.y
  std::unique_ptr<Block> block =
      BlockBuilder()
          .AddInput("bar", type_factory.MakePrimitiveType())
          .AddOutput("foo", type_factory.MakePrimitiveType())
          .AddImplementation([tag_claim_op, make_foo_op, read_field_op,
                              constant_op,
                              merge_op](BlockBuilder& builder, Block& block) {
            // Create values for bar.x and bar.y
            Value bar(value::BlockArgument(block, "bar"));

            // Build claim foo.a is "UserSelection".
            //
            // create "userSelection" predicate. TODO: Store the actual
            // ir::Predicate instance.
            const Operation& predicate_op = builder.AddOperation(
                constant_op,
                {{"predicate", StringAttribute::Create("userSelection")}}, {});
            Value userSelection =
                Value(value::OperationResult(predicate_op, "result"));

            const Operation& bar_x_op = builder.AddOperation(
                read_field_op, {{"field", StringAttribute::Create("x")}},
                {
                    {"input", bar},
                });

            const Operation& bar_y_op = builder.AddOperation(
                read_field_op, {{"field", StringAttribute::Create("y")}},
                {
                    {"input", bar},
                });

            Value bar_x = Value(value::OperationResult(bar_x_op, "output"));
            Value bar_y = Value(value::OperationResult(bar_y_op, "output"));

            const Operation& pred_input = builder.AddOperation(
                merge_op, {},
                {{"0", bar_x}, {"1", bar_y}, {"2", Value(value::Any())}});
            // Input of tag claim operation is mapped to both bar.x and
            // bar.y to reflect the fact that foo.a may get its value
            // from both.  We also add a "Any()" as an input to indicate
            // that foo.a would also be created from something other
            // than `bar.x` and `bar.y`.
            const Operation& foo_a = builder.AddOperation(
                tag_claim_op, {},
                {
                    {"input",
                     Value(value::OperationResult(pred_input, "output"))},
                    {"predicate", userSelection},
                });

            // Create `foo` for the output.
            const Operation& foo = builder.AddOperation(
                make_foo_op, {},
                {{"a", Value(value::OperationResult(foo_a, "output"))},
                 {"b", bar_y}});

            // Assign the foo constructed from these operations to be the
            // output of this particle.
            builder.AddResult("foo",
                              Value(value::OperationResult(foo, "value")));
          })
          .build();
  const Operator& particle_p1 =
      context.RegisterOperator(std::make_unique<Operator>("arcs.MakeParticle"));
  //, std::move(block))
  // ASSERT_NE(particle_p1, nullptr);

  // Instances of particle are represented as operations.
  auto input_storage =
      std::make_unique<Storage>("input_data", type_factory.MakePrimitiveType());
  auto output_storage = std::make_unique<Storage>(
      "output_data", type_factory.MakePrimitiveType());

  // Recipe R
  // P1
  //  bar: reads h1
  //  foo: writes h2
  std::unique_ptr<Operation> particle_instance(
      new Operation(nullptr, particle_p1, {},
                    {{"bar", {Value(value::StoredValue(*input_storage))}}}));
  std::unique_ptr<Operation> write_storage(new Operation(
      nullptr, write_op, {},
      {{"src", {Value(value::OperationResult(*particle_instance, "foo"))}},
       {"tgt", {Value(value::StoredValue(*output_storage))}}}));

  ASSERT_EQ(particle_instance->op().name(), "arcs.MakeParticle");
  ASSERT_EQ(write_storage->op().name(), "core.write");

  SSANames ssa_names;
  for (const auto& op : block->operations()) {
    LOG(INFO) << op->ToString(ssa_names);
  }
}

}  // namespace raksha::ir
