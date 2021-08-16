#ifndef SRC_IR_FIELD_SELECTOR_H_
#define SRC_IR_FIELD_SELECTOR_H_

#include <string>

namespace raksha::ir {

class FieldSelector {
 public:
  FieldSelector(const std::string field_name) : field_name_(field_name) {}

  std::string ToString() const;

  bool operator==(const FieldSelector &other) const {
    return field_name_ == other.field_name_;
  }

  template<typename H>
  friend H AbslHashValue(H h, const FieldSelector &field_selector) {
    return H::combine(std::move(h), field_selector.field_name_);
  }

 private:
  std::string field_name_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_FIELD_SELECTOR_H_
