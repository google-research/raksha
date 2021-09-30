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
//----------------------------------------------------------------------------

#ifndef SRC_IR_PREDICATES_PREDICATE_H_
#define SRC_IR_PREDICATES_PREDICATE_H_

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "src/ir/access_path.h"

// This file includes Predicate and its subclasses. These classes are boolean
// expressions upon the presence of tags. We place them all here in one file
// because they are small classes and related enough that it improves
// readability to have them in one place.

namespace raksha::ir::predicates {

// A Predicate is a boolean expression that can occur upon various Raksha IR
// structures. Currently, all leaf expressions speak of whether a particular
// tag is present. Inner nodes include the full range of boolean expressions
// you might expect (and, or, not) plus the slightly more exotic implies.
class Predicate {
 public:
  virtual ~Predicate() {}
  // Turns this predicate into a rule body that can be used for checking if
  // the given condition holds.
  virtual std::string ToDatalogRuleBody(const AccessPath &ap) const = 0;
};

class And : public Predicate {
 public:
  explicit And(std::unique_ptr<Predicate> lhs, std::unique_ptr<Predicate> rhs)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }

  virtual ~And() { }

  std::string ToDatalogRuleBody(const AccessPath &ap) const override {
    constexpr absl::string_view kFormatString = R"(((%s), (%s)))";
    return absl::StrFormat(
        kFormatString, lhs_->ToDatalogRuleBody(ap),
        rhs_->ToDatalogRuleBody(ap));
  }

 private:
  std::unique_ptr<Predicate> lhs_;
  std::unique_ptr<Predicate> rhs_;
};

// An implies statement, usually written:
//
// antecedent => consequent
//
// means that if the antecedent holds, the consequent should also hold.
// Viewed as a boolean expression in its own right, this means that the
// implication is true if both the antecedent and the consequent are true OR
// if the antecedent is false.
class Implies : public Predicate {
 public:
  explicit Implies(
      std::unique_ptr<Predicate> antecedent,
      std::unique_ptr<Predicate> consequent) :
      antecedent_(std::move(antecedent)), consequent_(std::move(consequent)) { }

  virtual ~Implies() { }

  std::string ToDatalogRuleBody(const AccessPath &ap) const override {
    // To make it so that Souffle has first class information that our
    // antecedent is the same in both places it is used, we assign it to a
    // variable within the rule.
    std::string antecedent_str = antecedent_->ToDatalogRuleBody(ap);
    constexpr absl::string_view kFormatString =
        R"(!(%s); ((%s), (%s)))";
    return absl::StrFormat(
        kFormatString, antecedent_str, antecedent_str,
        consequent_->ToDatalogRuleBody(ap));
  }

 private:
  std::unique_ptr<Predicate> antecedent_;
  std::unique_ptr<Predicate> consequent_;
};

// A Not boolean expression in a predicate. Just negates the inner predicate.
class Not : public Predicate {
 public:
  explicit Not(std::unique_ptr<Predicate> negated_predicate)
    : negated_predicate_(std::move(negated_predicate)) { }

  virtual ~Not() { }

  std::string ToDatalogRuleBody(const AccessPath &ap) const override {
    constexpr absl::string_view kFormatString = R"(!(%s))";
    return absl::StrFormat(
        kFormatString, negated_predicate_->ToDatalogRuleBody(ap));
  }

 private:
  std::unique_ptr<Predicate> negated_predicate_;
};

// A boolean or expression on two predicates.
class Or : public Predicate {
 public:
  explicit Or(std::unique_ptr<Predicate> lhs, std::unique_ptr<Predicate> rhs)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)) { }

  virtual ~Or() { }

  std::string ToDatalogRuleBody(const AccessPath &ap) const override {
    constexpr absl::string_view kFormatString = R"(((%s); (%s)))";
    return absl::StrFormat(kFormatString, lhs_->ToDatalogRuleBody(ap),
                           rhs_->ToDatalogRuleBody(ap));
  }

 private:
  std::unique_ptr<Predicate> lhs_;
  std::unique_ptr<Predicate> rhs_;
};

// A boolean expression that is true if a tag is possibly present on an
// AccessPath, false if it cannot possibly be present. The leaf of the
// predicate tree.
class TagPresence : public Predicate {
 public:
  explicit TagPresence(std::string tag) : tag_(std::move(tag)) { }

  virtual ~TagPresence() {}

  // The tag presence just turns into a check for the tag on the access_path
  // in the mayHaveTag relation.
  std::string ToDatalogRuleBody(const AccessPath &access_path) const override {
    constexpr absl::string_view kFormatStr = R"(mayHaveTag("%s", "%s"))";
    return absl::StrFormat(kFormatStr, access_path.ToString(), tag_);
  }

 private:
  std::string tag_;
};

}  // namespace raksha::ir::predicates

#endif  // SRC_IR_PREDICATES_PREDICATE_H_
