#ifndef SRC_IR_PROTO_TAG_CHECK_H_
#define SRC_IR_PROTO_TAG_CHECK_H_

#include "src/ir/tag_check.h"
#include "src/ir/proto/access_path.h"
#include "src/ir/proto/predicate.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

TagCheck Decode(const arcs::CheckProto &check_proto) {
  CHECK(check_proto.has_access_path())
    << "`Check` proto missing required field access_path!";
  AccessPath access_path = Decode(check_proto.access_path());
  CHECK(check_proto.has_predicate())
    << "`Check` proto missing required field predicate!";
  return TagCheck(std::move(access_path), Decode(check_proto.predicate()));
}

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_TAG_CHECK_H_
