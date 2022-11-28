#include "src/ir/value_string_converter.h"

#include "src/common/utils/overloaded.h"
#include "src/ir/value.h"

namespace raksha::ir {
std::string ValueStringConverter::ToString(Value value) const {
  return std::visit(
      raksha::utils::overloaded{
          [](const value::Any& any) { return any.ToString(); },
          [](const value::StoredValue& stored_value) {
            return stored_value.ToString();
          },
          [&ssa_names =
               ssa_names_](const value::BlockArgument& block_argument) {
            return ssa_names.GetOrCreateID(Value(block_argument));
          },
          [&ssa_names =
               ssa_names_](const value::OperationResult& operation_result) {
            return ssa_names.GetOrCreateID(Value(operation_result));
          }},
      value.value_);
}
}  // namespace raksha::ir
