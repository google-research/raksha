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
#ifndef SRC_IR_UTILS_OVERLOADED_H_
#define SRC_IR_UTILS_OVERLOADED_H_

namespace raksha::utils {

// See overloaded_test.cc for an example usage.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

// Allows using in-place lambas for the different types of a variant when
// visiting a std::variant type. This won't be needed from C++20.
//
// Suppose we have the following variant type:
//    using var_t = std::variant<int, long, double, std::string>;
//
// Here is an example of how `overloaded` can be used to use in-place lambas:
//
//  . var_t v = ...
//    auto res = std::visit(overloaded {
//      [](auto arg) { return generic_call(arg); },
//      [](double arg) { return double_call(args); },
//      [](const std::string& arg) { return string_call(arg); }
//    }, v);
//
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace raksha::utils

#endif  // SRC_IR_UTILS_OVERLOADED_H_
