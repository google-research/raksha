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

namespace raksha::utils {

template <typename T, typename U, typename F>
std::vector<U> MapIter(const std::vector<T>& input, F f) {
  std::vector<U> result;
  result.reserve(input.size());
  for (auto& entity : input) {
    result.push_back(f(entity));
  }
  return result;
}

}  // namespace raksha::utils

#endif  // SRC_UTILS_MAP_ITER_H_
