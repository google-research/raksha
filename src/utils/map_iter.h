/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.
#ifndef SRC_UTILS_MAP_ITER_H_
#define SRC_UTILS_MAP_ITER_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "src/common/logging/logging.h"

namespace raksha::utils {

enum class CollisionPolicy {
  kError,
  kIgnore,
};

// A version of `Map` that iterates over a reference to a container to
// populate a resultant container. The input container is taken as a const
// reference here.
template <typename ResultT, typename InputT, typename F>
ResultT MapIterRef(const InputT& input, F f,
                   CollisionPolicy collision_policy = CollisionPolicy::kError) {
  ResultT result;
  result.reserve(input.size());
  for (auto& entity : input) {
    uint64_t pre_insert_size = result.size();
    result.insert(result.end(), f(entity));
    // The usual way to check whether an insert succeeded or not would be to
    // check the return value of `insert`. Unfortunately, this has different
    // forms for `std::vector` versus maps or sets. Therefore, we just check
    // that the size of the resultant collection has increased to ensure that
    // the insert succeeded.
    CHECK((collision_policy != CollisionPolicy::kError) ||
          (pre_insert_size < result.size()))
        << "Found collision when mapping a container.";
  }
  return result;
}

// A version of `Map` that iterates over a reference to a container to
// populate a resultant container. The input container is taken as a value
// here, which is better for if the input can be moved or is a temporary.
template <typename ResultT, typename InputT, typename F>
ResultT MapIterVal(InputT input, F f,
                   CollisionPolicy collision_policy = CollisionPolicy::kError) {
  ResultT result;
  result.reserve(input.size());
  for (auto iter = std::make_move_iterator(input.begin());
       iter != std::make_move_iterator(input.end()); ++iter) {
    uint64_t pre_insert_size = result.size();
    result.insert(result.end(), f(*iter));
    // The usual way to check whether an insert succeeded or not would be to
    // check the return value of `insert`. Unfortunately, this has different
    // forms for `std::vector` versus maps or sets. Therefore, we just check
    // that the size of the resultant collection has increased to ensure that
    // the insert succeeded.
    CHECK((collision_policy != CollisionPolicy::kError) ||
          (pre_insert_size < result.size()))
        << "Found collision when mapping a container.";
  }
  return result;
}

}  // namespace raksha::utils

#endif  // SRC_UTILS_MAP_ITER_H_
