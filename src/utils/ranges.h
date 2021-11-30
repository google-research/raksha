//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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
//-----------------------------------------------------------------------------
#ifndef SRC_IR_UTILS_RANGES_H_
#define SRC_IR_UTILS_RANGES_H_

#include <iterator>

namespace raksha::utils {

// A utility class to create a range from iterators. Do not construct this class
// directly, instead use the `make_range` method below.
template <typename I>
class iterator_range {
 public:
  using const_iterator = I;
  using value_type = typename std::iterator_traits<I>::value_type;

  iterator_range() = default;
  iterator_range(I begin, I end) : begin_(begin), end_(end) {}

  I begin() const { return begin_; }
  I end() const { return end_; }
  bool empty() const { return begin() == end(); }

 private:
  I begin_;
  I end_;
};

template <typename I>
inline iterator_range<I> make_range(I begin, I end) {
  return iterator_range<I>(begin, end);
}

}  // namespace raksha::utils

#endif  // SRC_IR_UTILS_RANGES_H_
