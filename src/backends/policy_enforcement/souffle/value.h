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

#ifndef SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_VALUE_H_
#define SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_VALUE_H_

#include <string>
#include <tuple>
#include <variant>

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"

namespace raksha::backends::policy_enforcement::souffle {

// The common supertype of any "fully fledged" Souffle value type. Anything
// descending form `Value` should be able to be used in a `.type` declaration.
// By this logic, `number`, `symbol`, any record, and any ADT are `Value`s, but
// individual branches of an ADT are not.
class Value {
 public:
  virtual std::string ToDatalogString() const = 0;
};

// This is the common supertype of any Souffle value type that can
// participate in an ADT. This is all Values except Records. Values that can
// participate in an ADT must have a virtual destructor.
class AdtMemberValue : public Value {
 public:
  virtual ~AdtMemberValue() {}
};

// Corresponds to Souffle's `number` type.
class Number : public AdtMemberValue {
 public:
  explicit Number(int64_t value) : number_value_(value) {}
  virtual ~Number() {}

  std::string ToDatalogString() const override {
    return std::to_string(number_value_);
  }

 private:
  int64_t number_value_;
};

// Corresponds to Souffle's `symbol` type.
class Symbol : public AdtMemberValue {
 public:
  explicit Symbol(absl::string_view value) : symbol_value_(value) {}
  virtual ~Symbol() {}

  std::string ToDatalogString() const override {
    return absl::StrFormat(R"("%s")", symbol_value_);
  }

 private:
  std::string symbol_value_;
};

// A Record is, in fact, a value pointing to a tuple of values that
// constitutes a record. This is necessary because Souffle conflates references
// and the objects to which those references point. In the case that a type has
// a recursive definition (common with linked lists), trying to implement this
// without a pointer would cause layout issues.
template <class... RecordFieldValueTypes>
class Record : public Value {
 public:
  explicit Record() : record_arguments_() {}
  explicit Record(RecordFieldValueTypes &&...args)
      : record_arguments_(
            std::make_unique<std::tuple<RecordFieldValueTypes...>>(
                std::forward<RecordFieldValueTypes>(args)...)) {}

  std::string ToDatalogString() const override {
    if (!record_arguments_) return "nil";
    return absl::StrFormat(
        "[%s]", absl::StrJoin(*record_arguments_, ", ",
                              [](std::string *out, const auto &arg) {
                                absl::StrAppend(out, arg.ToDatalogString());
                              }));
  }

 private:
  std::unique_ptr<std::tuple<RecordFieldValueTypes...>> record_arguments_;
};

class Adt : public AdtMemberValue {
 public:
  explicit Adt(absl::string_view branch_name) : branch_name_(branch_name) {}

  std::string ToDatalogString() const {
    return absl::StrFormat(
        "$%s{%s}", branch_name_,
        absl::StrJoin(members_, ", ", [](std::string *str, const auto &arg) {
          absl::StrAppend(str, arg->ToDatalogString());
        }));
  }

 protected:
  std::vector<std::unique_ptr<AdtMemberValue>> members_;

 private:
  absl::string_view branch_name_;
};

}  // namespace raksha::backends::policy_enforcement::souffle

#endif  // SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_VALUE_H_
