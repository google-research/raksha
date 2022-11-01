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

#ifndef SRC_IR_DATALOG_INPUT_RELATION_FACT_H_
#define SRC_IR_DATALOG_INPUT_RELATION_FACT_H_

#include <utility>

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "src/ir/datalog/value.h"

namespace raksha::ir::datalog {

// A fact that is built to be fed into an input relation for Souffle.
// Although the `Rule` class can also express a `Fact` by having no RHS, this
// is explicitly to prepare input to a Souffle program. You can think of the
// difference between this `Fact` and a `Rule` fact as a difference between
// data and code.
template <class... RelationParameterTypes>
class InputRelationFact {
 public:
  InputRelationFact(RelationParameterTypes &&...args)
      : relation_arguments_(std::forward<RelationParameterTypes>(args)...) {}

  virtual std::string_view GetRelationName() const = 0;

  std::string ToDatalogString() const {
    return absl::StrFormat(R"(%s(%s).)", GetRelationName(),
                           absl::StrJoin(relation_arguments_, ", ",
                                         [](std::string *str, const auto &arg) {
                                           absl::StrAppend(
                                               str, arg.ToDatalogString());
                                         }));
  }

  // Returns string representation of a fact that can be used in an input
  // relations file.
  std::string ToDatalogFactsFileString() const {
    return absl::StrJoin(relation_arguments_, "; ",
                         [](std::string *str, const auto &arg) {
                           absl::StrAppend(str, arg.ToDatalogString());
                         });
  }

 private:
  std::tuple<RelationParameterTypes...> relation_arguments_;
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_INPUT_RELATION_FACT_H_
