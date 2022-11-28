#ifndef SRC_IR_VALUE_PRINTER_H_
#define SRC_IR_VALUE_PRINTER_H_

#include "absl/log/die_if_null.h"
#include "src/ir/ssa_names.h"

namespace raksha::ir {

class ValueStringConverter {
 public:
  explicit ValueStringConverter(SsaNames* ssa_names)
      : ssa_names_(*ABSL_DIE_IF_NULL(ssa_names)) {}

  std::string ToString(Value value) const;

 private:
  SsaNames& ssa_names_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_VALUE_PRINTER_H_
