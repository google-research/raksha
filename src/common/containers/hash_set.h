#ifndef THIRD_PARTY_RAKSHA_SRC_COMMON_CONTAINERS_HASH_SET_H_
#define THIRD_PARTY_RAKSHA_SRC_COMMON_CONTAINERS_HASH_SET_H_

#include "absl/container/flat_hash_set.h"

namespace raksha::common::containers {

template <class Value>
using HashSet = absl::flat_hash_set<Value>;

}  // namespace raksha::common::containers

#endif  // THIRD_PARTY_RAKSHA_SRC_COMMON_CONTAINERS_HASH_SET_H_
