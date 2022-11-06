#ifndef SRC_COMMON_CONTAINERS_HASH_MAP_H_
#define SRC_COMMON_CONTAINERS_HASH_MAP_H_

#include "absl/container/flat_hash_map.h"

namespace raksha::common::containers {

template <class Key, class Value>
using HashMap = absl::flat_hash_map<Key, Value>;

}  // namespace raksha::common::containers

#endif  // SRC_COMMON_CONTAINERS_HASH_MAP_H_
