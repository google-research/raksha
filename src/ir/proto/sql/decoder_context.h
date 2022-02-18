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

#ifndef SRC_IR_PROTO_SQL_MVP_DECODER_CONTEXT_H_
#define SRC_IR_PROTO_SQL_MVP_DECODER_CONTEXT_H_

#include <memory>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "src/ir/types/type_factory.h"
#include "src/ir/ir_context.h"
#include "src/ir/block_builder.h"
#include "src/ir/storage.h"
#include "src/ir/value.h"

namespace raksha::ir::proto::sql {

// Context and helper methods on context state for decoding SQL MVP proto
// objects.
class DecoderContext {
 public:
  static constexpr absl::string_view kTagClaimOperatorName = "tag_claim";
  static constexpr absl::string_view kAddedTagAttributeName = "added_tag";
  static constexpr absl::string_view kTagKindAttributeName = "tag_kind";
  static constexpr absl::string_view kConfidentialityTagAttrValue =
      "confidentiality";
  static constexpr absl::string_view kIntegrityTagAttrValue = "integrity";
  static constexpr absl::string_view kTagClaimInputValueName = "input";
  static constexpr absl::string_view kTagClaimOutputName = "out";

  explicit DecoderContext(IRContext &ir_context) : ir_context_(ir_context) { }

  const Storage &GetOrCreateStorage(absl::string_view storage_name) {
    if (ir_context_.IsRegisteredStorage(storage_name)) {
      return ir_context_.GetStorage(storage_name);
    } else {
      return ir_context_.RegisterStorage(
          std::make_unique<Storage>(
              storage_name,
              // Always use PrimitiveType, as we are ignoring types for the
              // moment.
              ir_context_.type_factory().MakePrimitiveType()));
    }
  }

  ir::Value ClaimTagOnValue(
      ir::Value value, absl::string_view tag, bool is_integrity) {
    const Operator &tag_claim_op =
        (ir_context_.IsRegisteredOperator(kTagClaimOperatorName))
          ? ir_context_.GetOperator(kTagClaimOperatorName)
          : ir_context_.RegisterOperator(
              std::make_unique<Operator>(kTagClaimOperatorName));

    return ir::Value(value::OperationResult(
        global_sql_block_builder_.AddOperation(
                tag_claim_op,
                NamedAttributeMap{
                  {std::string(kAddedTagAttributeName),
                   StringAttribute::Create(tag)},
                  {std::string(kTagKindAttributeName),
                   StringAttribute::Create(
                       (is_integrity)
                        ? kIntegrityTagAttrValue
                        : kConfidentialityTagAttrValue)}},
                NamedValueMap{{std::string(kTagClaimInputValueName), value}}),
        kTagClaimOutputName));
  }

  std::unique_ptr<Block> FinishGlobalSqlBlock() {
    return global_sql_block_builder_.build();
  }
 private:
  BlockBuilder global_sql_block_builder_;
  IRContext &ir_context_;
};

}  // namespace raksha::ir::proto::sql

#endif  // SRC_IR_PROTO_SQL_MVP_DECODER_CONTEXT_H_
