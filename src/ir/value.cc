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

#include "src/ir/value.h"

namespace raksha::ir {

using value::Any;
using value::BlockArgument;
using value::OperationResult;
using value::StoredValue;

bool BlockArgument::operator==(const Value &other_val) const {
  const BlockArgument *other_block_arg = other_val.If<BlockArgument>();
  if (other_block_arg == nullptr) return false;
  return NamedValue::NamedValuesEq(*this, *other_block_arg);
}

bool OperationResult::operator==(const Value &other_val) const {
  const OperationResult *other_operation_result =
      other_val.If<OperationResult>();
  if (other_operation_result == nullptr) return false;
  return NamedValue::NamedValuesEq(*this, *other_operation_result);
}

bool StoredValue::operator==(const Value &other) const {
  const StoredValue *other_stored_value = other.If<StoredValue>();
  if (other_stored_value == nullptr) return false;
  return storage_ == other_stored_value->storage_;
}

// Because `Any` has no internal structure, just return true if `other` is
// also an `Any`.
bool Any::operator==(const Value &other) const {
  return other.If<Any>() != nullptr;
}

}  // namespace raksha::ir
