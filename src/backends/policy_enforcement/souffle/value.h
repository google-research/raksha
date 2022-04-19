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

 private:
};

// Corresponds to Souffle's `number` type.
class Number : public Value {
 public:
  explicit Number(int64_t value) : number_value_(value) {}

  std::string ToDatalogString() const override {
    return std::to_string(number_value_);
  }

 private:
  int64_t number_value_;
};

// Corresponds to Souffle's `symbol` type.
class Symbol : public Value {
 public:
  explicit Symbol(absl::string_view value) : symbol_value_(value) {}

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

// An ADT branch with the branch name and argument values erased from the
// template, instead living as dynamic entries in the `branch_name_` and
// `args_` fields. This is necessary because a declaration that asks for an
// ADT will accept any of the branches of that ADT, so static typing
// information is lost at the declaration site. This keeps its args as a
// vector of unique pointers to `Value`s; this is necessary because ADTs can
// have recursive definitions (it is common for branches of an ADT to have
// members also of that ADT type) so we need to use pointers to arguments to
// get a well-formed layout.
template <const char *kAdtName>
class ErasedAdtBranch {
 public:
  template <class... AdtBranchArgs>
  ErasedAdtBranch(absl::string_view branch_name, AdtBranchArgs &&...branch_args)
      : branch_name_(branch_name),
        args_(MakeUniquePtrValueVectorFromValues(
            std::forward<AdtBranchArgs>(branch_args)...)) {}

  std::string ToDatalogString() const {
    return absl::StrFormat(
        "$%s{%s}", branch_name_,
        absl::StrJoin(args_, ", ", [](std::string *str, const auto &arg) {
          absl::StrAppend(str, arg->ToDatalogString());
        }));
  }

 private:
  static void MakeUniquePtrValueVectorFromValuesInner(
      std::vector<std::unique_ptr<Value>> &result) {}

  template <class ValueArg, class... ValueArgs>
  static void MakeUniquePtrValueVectorFromValuesInner(
      std::vector<std::unique_ptr<Value>> &result, ValueArg &&value,
      ValueArgs &&...values) {
    result.push_back(std::make_unique<ValueArg>(std::forward<ValueArg>(value)));
    MakeUniquePtrValueVectorFromValuesInner(result,
                                            std::forward<ValueArgs>(values)...);
  }

  // This helper is used to unwind the variadic parameter pack used to hold the
  // statically arguments on an incoming `AdtBranch` and turn them into a
  // `vector` of arguments, where the only static type information known is
  // that they descend from `Value`.
  template <class... ValueArgs>
  static std::vector<std::unique_ptr<Value>> MakeUniquePtrValueVectorFromValues(
      ValueArgs &&...values) {
    std::vector<std::unique_ptr<Value>> result;
    MakeUniquePtrValueVectorFromValuesInner(result,
                                            std::forward<ValueArgs>(values)...);
    return result;
  }

  absl::string_view branch_name_;
  std::vector<std::unique_ptr<Value>> args_;
};

// An ADT branch with full static type information. This is basically used to
// get a friendly, type-safe constructor around an `ErasedAdtBranch`.
template <const char *kAdtName, const char *kAdtBranchName,
          class... AdtBranchArgumentTypes>
class AdtBranch {
 public:
  explicit AdtBranch(AdtBranchArgumentTypes &&...args)
      : erased_branch_(kAdtBranchName,
                       std::forward<AdtBranchArgumentTypes>(args)...) {}

  std::string ToDatalogString() const {
    return erased_branch_.ToDatalogString();
  }

  template <const char *kAdtFriendName>
  friend class Adt;

 private:
  ErasedAdtBranch<kAdtName> erased_branch_;
};

// An ADT, which may hold any branch belonging to that ADT. Because of the
// type system tricks we are playing, we express this as a wrapper holding a
// single `ErasedAdtBranch`, which must statically indicate that it belongs
// to this ADT.
template <const char *kAdtName>
class Adt : public Value {
 public:
  Adt(ErasedAdtBranch<kAdtName> branch) : branch_(std::move(branch)) {}

  template <const char *kAdtBranchName, class... AdtBranchArgumentTypes>
  Adt(AdtBranch<kAdtName, kAdtBranchName, AdtBranchArgumentTypes...> branch)
      : branch_(std::move(branch.erased_branch_)) {}

  std::string ToDatalogString() const override {
    return branch_.ToDatalogString();
  }

 private:
  ErasedAdtBranch<kAdtName> branch_;
};

}  // namespace raksha::backends::policy_enforcement::souffle

#endif  // SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_VALUE_H_
