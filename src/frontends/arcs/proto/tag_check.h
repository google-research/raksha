#ifndef SRC_FRONTENDS_ARCS_PROTO_TAG_CHECK_H_
#define SRC_FRONTENDS_ARCS_PROTO_TAG_CHECK_H_

#include "src/frontends/arcs/proto/access_path.h"
#include "src/frontends/arcs/proto/predicate.h"
#include "src/frontends/arcs/tag_check.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::proto {

TagCheck Decode(const ::arcs::CheckProto &check_proto) {
  CHECK(check_proto.has_access_path())
      << "`Check` proto missing required field access_path!";
  AccessPath access_path = Decode(check_proto.access_path());
  CHECK(check_proto.has_predicate())
      << "`Check` proto missing required field predicate!";
  return TagCheck(std::move(access_path), Decode(check_proto.predicate()));
}

}  // namespace raksha::frontends::arcs::proto

#endif  // SRC_FRONTENDS_ARCS_PROTO_TAG_CHECK_H_
