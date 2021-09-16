#ifndef SRC_IR_TYPES_TYPE_H_
#define SRC_IR_TYPES_TYPE_H_

#include "absl/container/flat_hash_set.h"
#include "src/ir/access_path_selectors_set.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::types {

class Type {
 public:
  enum class Kind { kPrimitive, kEntity };

  // Create a Type from the given Arcs manifest TypeProto and return it via
  // an owning pointer. Will delegate to the various base types of Type
  // according to the oneof field data within TypeProto.
  static std::unique_ptr<Type> CreateFromProto(
      const arcs::TypeProto &type_proto);

  virtual ~Type() {}

  virtual raksha::ir::AccessPathSelectorsSet
    GetAccessPathSelectorsSet() const = 0;

  // Returns the kind of type.
  virtual Kind kind() const = 0;

  // Make an arcs::TypeProto from a Type.
  virtual arcs::TypeProto MakeProto() const = 0;
};

}  // namespace raksha::ir::types

#endif  // SRC_IR_TYPES_TYPE_H_
