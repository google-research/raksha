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
#ifndef SRC_COMMON_UTILS_TYPES_H_
#define SRC_COMMON_UTILS_TYPES_H_

#include <variant>

namespace raksha {
// Type alias for monostate that sounds less scary.
using Unit = std::monostate;

// An implementation of a static, type-level, 'if'.
// Useful for writing typesafe, code generically (especially for generic constness).
// Based on SAT's laser (specifically 'misc_utils.h').
template <bool Flag, typename T, typename F>
struct SelectType {
    using Result = T;
};

template <typename T, typename F>
struct SelectType<true, T, F> {
    using Result = T;
};

template <typename T, typename F>
struct SelectType<false, T, F> {
    using Result = F;
};

template <bool IsConst, typename T>
using CopyConst = typename SelectType<IsConst, const T, T>::Result;

}

#endif  // SRC_COMMON_UTILS_TYPES_H_
