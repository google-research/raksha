#ifndef SRC_IR_SELECTOR_H_
#define SRC_IR_SELECTOR_H_

#include <string>

#include "absl/types/variant.h"
#include "src/common/logging/logging.h"
#include "src/ir/field_selector.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir {

// An individual selector in an AccessPath descending down the type tree.
// This Selector is generic, and may contain any method of descending, such
// as field access, array access, tuple element access, etc.
//
// For now, however, we only have implemented the FieldSelector portion.
class Selector {
 public:

  // A factory creating a Selector from a Manifest proto selector. Currently
  // only implements the FieldSelector proto.
  static Selector CreateFromProto(arcs::AccessPathProto_Selector selector) {
    if (selector.has_field()) {
      return Selector(FieldSelector(selector.field()));
    }
    LOG(FATAL) << "Found a Selector with an unimplemented specific type.";
  }

  explicit Selector(FieldSelector field_selector)
    : specific_selector_(std::move(field_selector)) {}

  // Print the string representing a selector's participation in the
  // AccessPath. This will include the punctuation indicating the method of
  // selection (such as . for string, [ for index, etc) and the string
  // contents of the selector itself.
  std::string ToString() const {
    return absl::visit(
      [](const auto &specific_selector) {
          return specific_selector.ToString(); }, specific_selector_);
  }

  // Whether two selectors are equal. Will be true if they are the same type
  // of selector and those two types also compare equal.
  //
  // absl::variant has a pre-packaged operator equals that handles the dispatch
  // to the specific selector as we would expect.
  bool operator==(const Selector &other) const {
    return specific_selector_ == other.specific_selector_;
  }

  // Make a proto from the current selector. Just uses absl::visit to
  // delegate to the specific type of the Selector.
  arcs::AccessPathProto_Selector MakeProto() const {
    auto visitor = [](const auto &specific_selector) {
      return specific_selector.MakeProto(); };
    return absl::visit(visitor, specific_selector_);
  }

  template<typename H>
  friend H AbslHashValue(H h, const Selector &selector) {
    return H::combine(std::move(h), selector.specific_selector_);
  }


 private:
  // The variant storing the specific type of selector. We will dispatch
  // through this to perform all methods on the generic Selector.
  absl::variant<FieldSelector> specific_selector_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_SELECTOR_H_
