#ifndef SRC_IR_TYPES_TYPE_H_
#define SRC_IR_TYPES_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"

namespace raksha::ir::types {

class TypeBase {
 public:
  enum class Kind { kPrimitive, kEntity };

  virtual ~TypeBase() {}

  virtual raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet()
      const = 0;

  // Returns the kind of type.
  virtual Kind kind() const = 0;
};

class Type {
 public:
  Type(std::unique_ptr<TypeBase> type) : type_(std::move(type)) {}

  raksha::ir::AccessPathSelectorsSet GetAccessPathSelectorsSet() const {
    return type_->GetAccessPathSelectorsSet();
  }

  // TODO: This should be removed to hide the TypeBase.  method is added as a
  // stop-gap solution to work with the rest of the code base.
  const TypeBase& type_base() const { return *type_.get(); }

 private:
  std::unique_ptr<TypeBase> type_;
};


}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_TYPE_H_
