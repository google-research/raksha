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

#ifndef SRC_IR_PROTO_SQL_DECODER_CONTEXT_H_
#define SRC_IR_PROTO_SQL_DECODER_CONTEXT_H_

#include <memory>

#include "absl/container/flat_hash_map.h"
#include "src/ir/ir_context.h"
#include "src/ir/value.h"
#include "src/ir/proto/sql/sql_ir.pb.h"

namespace raksha::ir::proto::sql {

// Context specifically for the SQL proto decoding. This keeps track of state
// that is not required when building the IR in general, and thus would be
// inappropriate to put in IRContext.
class DecoderContext {
 public:
  DecoderContext(IRContext &ir_context)
    : ir_context_(ir_context),
      literal_operator_(ir_context_.RegisterOperator(
          std::make_unique<Operator>("sql.literal"))) { }

  // Get the `Storage` with the given name, create it otherwise. This is a
  // bit of a hack that we use here because SQL buries its global context
  // down at the bottom of its query expressions (unlike, say, LINQ which
  // puts it right at the beginning). This allows us to create `Storage`s for
  // columns as we see them. We will probably want to do something less lazy,
  // like registering storages in advance, for the mature implementation.
  const Storage &GetOrCreateStorage(absl::string_view storage_name) {
    return (ir_context_.IsRegisteredStorage(storage_name))
      ? ir_context_.GetStorage(storage_name)
      : ir_context_.RegisterStorage(std::make_unique<Storage>(
          storage_name, ir_context_.type_factory().MakePrimitiveType()));
  }

  // Create an `Operation` providing the literal value indicated by
  // `literal_str`. This shall create an `Operation` of `Operator` "sql.literal"
  // and with the literal value indicated in a `literal_str` attribute.
  const Operation &MakeLiteralOperation(absl::string_view literal_str) {
    operations_.push_back(std::make_unique<Operation>(
        /*parent=*/nullptr,
        literal_operator_,
        NamedAttributeMap{
          {"literal_str", StringAttribute::Create(literal_str)}},
        NamedValueMap{}));
    return *operations_.back();
  }

  IRContext &ir_context() { return ir_context_; }

 private:
  IRContext &ir_context_;
  const Operator &literal_operator_;
  std::vector<std::unique_ptr<Operation>> operations_;
};

}  // namespace raksha::ir::proto::sql

#endif  // SRC_IR_PROTO_SQL_DECODER_CONTEXT_H_
