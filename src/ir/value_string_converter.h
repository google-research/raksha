#ifndef SRC_IR_VALUE_PRINTER_H_
#define SRC_IR_VALUE_PRINTER_H_

#include <string>

#include "src/ir/ssa_names.h"
#include "src/ir/value.h"

namespace raksha::ir {

std::string ValueToString(Value value, SsaNames &ssa_names);

std::string StoredValueToString(value::StoredValue stored_value);

}  // namespace raksha::ir

#endif  // SRC_IR_VALUE_PRINTER_H_
