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

#ifndef SRC_FRONTENDS_SQL_DECODER_CONTEXT_H_
#define SRC_FRONTENDS_SQL_DECODER_CONTEXT_H_

#include <memory>

#include "src/common/containers/hash_map.h"
#include "src/frontends/sql/ops/literal_op.h"
#include "src/frontends/sql/ops/sql_output_op.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql {
// Context specifically for the SQL proto decoding. This keeps track of state
// that is not required when building the IR in general, and thus would be
// inappropriate to put in IRContext.
class DecoderContext {
 public:
  static constexpr absl::string_view kDefaultOutputName = "out.0";

  DecoderContext(ir::IRContext &ir_context)
      : ir_context_(ir_context),
        literal_operator_(ir_context_.RegisterOperator(
            std::make_unique<ir::Operator>(OpTraits<LiteralOp>::kName))),
        merge_operator_(ir_context_.RegisterOperator(
            std::make_unique<ir::Operator>(OpTraits<MergeOp>::kName))),
        tag_transform_operator_(ir_context_.RegisterOperator(
            std::make_unique<ir::Operator>(OpTraits<TagTransformOp>::kName))),
        sql_output_operator_(ir_context_.RegisterOperator(
            std::make_unique<ir::Operator>(OpTraits<SqlOutputOp>::kName))),
        global_module_(),
        top_level_block_builder_() {}

  // Takes an `Operation` that should only return a single result and creates
  // the default `OperationResult` value for that `Operation`.
  static ir::Value WrapOperationResultValue(const ir::Operation &operation) {
    return ir::Value(ir::value::OperationResult(operation, kDefaultOutputName));
  }

  // Register a value as associated with a particular id.
  void RegisterValue(uint64_t id, ir::Value value) {
    auto insert_result = id_to_value.insert({id, std::move(value)});
    CHECK(insert_result.second)
        << "id_to_value map has more than one value associated with the id "
        << insert_result.first->first << ".";
  }

  // Get a copy of the value associated with a particular id. Assumes that the
  // ID is present; it will error if it is not.
  ir::Value GetValue(uint64_t id) const {
    CHECK(id != 0)
        << "Attempt to get a value with id 0, which is not a legal value id.";
    // We could use `at`, which would be more concise and would also fail if the
    // id is not present in the map, but it will not provide a nice error
    // message. Use `find` and `CHECK` to get a better error message.
    auto find_result = id_to_value.find(id);
    CHECK(find_result != id_to_value.end())
        << "Could not find a value with id " << id << ".";
    return find_result->second;
  }
  // Get the `Storage` with the given name, create it otherwise. This is a
  // bit of a hack that we use here because SQL buries its global context
  // down at the bottom of its query expressions (unlike, say, LINQ which
  // puts it right at the beginning). This allows us to create `Storage`s for
  // columns as we see them. We will probably want to do something less lazy,
  // like registering storages in advance, for the mature implementation.
  const ir::Storage &GetOrCreateStorage(absl::string_view storage_name) {
    return (ir_context_.IsRegisteredStorage(storage_name))
               ? *ABSL_DIE_IF_NULL(ir_context_.GetStorage(storage_name))
               : ir_context_.RegisterStorage(std::make_unique<ir::Storage>(
                     storage_name,
                     ir_context_.type_factory().MakePrimitiveType()));
  }

  // Create an `Operation` providing the literal value indicated by
  // `literal_str`. This shall create an `Operation` of `Operator`
  // "sql.literal" and with the literal value indicated in a `literal_str`
  // attribute.
  const ir::Operation &MakeLiteralOperation(absl::string_view literal_str) {
    return top_level_block_builder_.AddOperation<LiteralOp>(ir_context_,
                                                            literal_str);
  }

  const ir::Operation &MakeMergeOperation(
      std::vector<uint64_t> direct_input_ids,
      std::vector<uint64_t> control_input_ids);

  const ir::Operation &MakeTagTransformOperation(
      uint64_t transformed_value_id, absl::string_view rule_name,
      const common::containers::HashMap<std::string, uint64_t>
          &tag_preconditions);

  const ir::Operation &MakeSqlOutputOperation(ir::Value value) {
    return top_level_block_builder_.AddOperation<SqlOutputOp>(ir_context_,
                                                              value);
  }

  // Finish building the top level block and
  const ir::Block &BuildTopLevelBlock() {
    return global_module_.AddBlock(top_level_block_builder_.build());
  }

  ir::IRContext &ir_context() { return ir_context_; }

  const ir::Module &global_module() const { return global_module_; }

 private:
  ir::IRContext &ir_context_;
  // A mapping from an `id` to the value associated with that `id`. We store
  // `id`s as values because we will be returning copies of them, not
  // pointers to them.
  common::containers::HashMap<uint64_t, ir::Value> id_to_value;
  // An `Operator` that provides the value of a literal.
  const ir::Operator &literal_operator_;
  // An `Operator` that just merges values together. This will cause the
  // sets of incoming confidentiality tags to be unioned and all integrity
  // tags to be dropped.
  const ir::Operator &merge_operator_;
  // An `Operator` that may perform some transformation upon the tag state if
  // some indicated policy rule fires.
  const ir::Operator &tag_transform_operator_;
  // An `Operator` that indicates that the wrapped value is an output of a
  // top-level query.
  const ir::Operator &sql_output_operator_;
  // A global module to which we can add SQL IR nodes.
  ir::Module global_module_;
  // A BlockBuilder for building the top-level block.
  ir::BlockBuilder top_level_block_builder_;
};

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_DECODER_CONTEXT_H_
