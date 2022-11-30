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
//-----------------------------------------------------------------------------

#ifndef SRC_COMMON_UTILS_FOLD_H_
#define SRC_COMMON_UTILS_FOLD_H_

// This file contains a C++ implementation of the classic "fold" and
// "right_fold" higher-order functions. This is somewhat redundant with
// std::accumulate; unfortunately, std::accumulate requires its accumulator to
// be copyable until C++20.
//
// Note that "right_fold" is named "rfold". This is a bit of a pun: if you come
// from a functional languages background, the "r" naturally expands to "right",
// while if you are fluent with C++ iterators, "r" naturally means "reverse".
// In this context it actually means both: a right_fold achieved by using
// reverse iterators on the provided container.

#include <iterator>

namespace raksha::common::utils {

// An implementation of `fold` that takes as inputs an iterator to the beginning
// of some range and an iterator to the end of that range. This just folds using
// `fold_fn` from the beginning iterator to the end iterator with an initial
// value of `accumulated`. Corresponds very directly to `std::accumulate` with a
// custom `BinaryOperator`.
template <class I, class A, class F>
A fold(I begin_iter, I end_iter, A accumulated, F fold_fn) {
  for (; begin_iter != end_iter; ++begin_iter) {
    accumulated = fold_fn(std::move(accumulated), *begin_iter);
  }
  return accumulated;
}

// A left fold over the provided container using `fold_fn` with an initial value
// of `init`.
template <class C, class A, class F>
A fold(const C &container, A &&init, F &&fold_fn) {
  return fold(std::begin(container), std::end(container), std::forward<A>(init),
              std::forward<F>(fold_fn));
}

// A right (or reverse) fold over the provided container using `fold_fn` with an
// initial value of `init`.
template <class C, class A, class F>
A rfold(const C &container, A &&init, F &&fold_fn) {
  return fold(std::rbegin(container), std::rend(container),
              std::forward<A>(init), std::forward<F>(fold_fn));
}

}  // namespace raksha::common::utils

#endif  // SRC_COMMON_UTILS_FOLD_H_
