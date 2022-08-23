//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------
#include "src/frontends/arcs/tag_check.h"

#include "src/frontends/arcs/proto/access_path.h"
#include "src/frontends/arcs/proto/predicate.h"
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
