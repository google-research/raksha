#ifndef THIRD_PARTY_RAKSHA_SRC_COMMON_ABSL_SHIM_FLAT_HASH_SET_H_
#define THIRD_PARTY_RAKSHA_SRC_COMMON_ABSL_SHIM_FLAT_HASH_SET_H_

#include "absl/container/flat_hash_set.h"

namespace raksha::absl_shim {

template <typename T>
using flat_hash_set = absl::flat_hash_set<T>;

}  // namespace raksha::absl_shim

#endif  // THIRD_PARTY_RAKSHA_SRC_COMMON_ABSL_SHIM_FLAT_HASH_SET_H_
