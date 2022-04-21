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

#ifndef SRC_IR_DATALOG_FACT_H_
#define SRC_IR_DATALOG_FACT_H_

#include <utility>

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "src/ir/datalog/value.h"

namespace raksha::ir::datalog {

template <const char *kRelationName, class... RelationParameterTypes>
class Fact {
 public:
  Fact(RelationParameterTypes &&...args)
      : relation_arguments_(std::forward<RelationParameterTypes>(args)...) {}

  std::string ToDatalogString() const {
    return absl::StrFormat(R"(%s(%s).)", kRelationName,
                           absl::StrJoin(relation_arguments_, ", ",
                                         [](std::string *str, const auto &arg) {
                                           absl::StrAppend(
                                               str, arg.ToDatalogString());
                                         }));
  }

 private:
  std::tuple<RelationParameterTypes...> relation_arguments_;
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_FACT_H_
