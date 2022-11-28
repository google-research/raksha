#include "src/ir/value_string_converter.h"

#include "src/common/utils/overloaded.h"
#include "src/ir/storage.h"
#include "src/ir/value.h"

namespace raksha::ir {
std::string ValueToString(Value value, SsaNames& ssa_names) {
  return std::visit(
      raksha::utils::overloaded{
          [](const value::Any& any) { return any.ToString(); },
          [](const value::StoredValue& stored_value) {
            return StoredValueToString(stored_value);
          },
          [&ssa_names](const value::BlockArgument& block_argument) {
            return ssa_names.GetOrCreateID(Value(block_argument));
          },
          [&ssa_names](const value::OperationResult& operation_result) {
            return ssa_names.GetOrCreateID(Value(operation_result));
          }},
      value.value_);
}

std::string StoredValueToString(value::StoredValue stored_value) {
  return stored_value.storage().ToString();
}
}  // namespace raksha::ir
