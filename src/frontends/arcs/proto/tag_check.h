#ifndef SRC_FRONTENDS_ARCS_PROTO_TAG_CHECK_H_
#define SRC_FRONTENDS_ARCS_PROTO_TAG_CHECK_H_

#include "src/frontends/arcs/tag_check.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::frontends::arcs::proto {

TagCheck Decode(const ::arcs::CheckProto &check_proto);

}  // namespace raksha::frontends::arcs::proto

#endif  // SRC_FRONTENDS_ARCS_PROTO_TAG_CHECK_H_
