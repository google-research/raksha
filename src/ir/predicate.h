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

namespace raksha::ir {

enum PredicateKind {
  kAnd,
  kImplies,
  kNot,
  kOr,
  kTagPresence,
};

// Forward declaration to break inclusion cycle.
class PredicateArena;

// A Predicate is a boolean expression that can occur upon various Raksha IR
// structures. Currently, all leaf expressions speak of whether a particular
// tag is present. Inner nodes include the full range of boolean expressions
// you might expect (and, or, not) plus the slightly more exotic implies.
class Predicate {
 public:
  virtual ~Predicate() {}
  // Turns this predicate into a rule body that can be used for checking if
  // the given condition holds.
  virtual std::string ToDatalogRuleBody(
      const AccessPath &ap, const DatalogPrintContext &ctxt) const = 0;
  virtual PredicateKind GetPredicateKind() const = 0;
  virtual bool operator==(Predicate const &other) const = 0;

  // Implement a dynamic downcast operation for Predicates. Will return a
  // downcast to the given type if the predicate is that type, nullptr
  // otherwise.
  template<class T>
  static const T *DynCast(const Predicate &pred) {
    if (pred.GetPredicateKind() == T::GetKind()) {
      return static_cast<const T *>(&pred);
    } else {
      return nullptr;
    }
  }
};

class And : public Predicate {
 public:
  static const And *Create(
      PredicateArena &pred, const Predicate *lhs, const Predicate *rhs);
  virtual ~And() { }

  std::string ToDatalogRuleBody(
      const AccessPath &ap, const DatalogPrintContext &ctxt) const override {
    constexpr absl::string_view kFormatString = R"(((%s), (%s)))";
    return absl::StrFormat(
        kFormatString, lhs_->ToDatalogRuleBody(ap, ctxt),
        rhs_->ToDatalogRuleBody(ap, ctxt));
  }

  static PredicateKind GetKind() { return kAnd; }

  PredicateKind GetPredicateKind() const override { return GetKind(); }

  bool operator==(const Predicate &other) const override {
    const And *other_and = Predicate::DynCast<And>(other);
    if (other_and == nullptr) return false;
    return (*lhs_ == *other_and->lhs_) && (*rhs_ == *other_and->rhs_);
  }
 private:
  explicit And(const Predicate *lhs, const Predicate *rhs)
    : lhs_(lhs), rhs_(rhs) { }

  const Predicate *lhs_;
  const Predicate *rhs_;
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
  static const Implies *Create(
      PredicateArena &arena,
      const Predicate *antecedent, const Predicate *consequent);

  virtual ~Implies() { }

  std::string ToDatalogRuleBody(
      const AccessPath &ap, const DatalogPrintContext &ctxt) const override {
    std::string antecedent_str = antecedent_->ToDatalogRuleBody(ap, ctxt);
    constexpr absl::string_view kFormatString =
        R"(!(%s); ((%s), (%s)))";
    return absl::StrFormat(
        kFormatString, antecedent_str, antecedent_str,
        consequent_->ToDatalogRuleBody(ap, ctxt));
  }

  static PredicateKind GetKind() {
    return PredicateKind::kImplies;
  }

  PredicateKind GetPredicateKind() const override { return GetKind(); }

  bool operator==(const Predicate &other) const override {
    const Implies *other_implies = Predicate::DynCast<Implies>(other);
    if (other_implies == nullptr) { return false; }
    return (*antecedent_ == *other_implies->antecedent_) &&
      (*consequent_ == *other_implies->consequent_);
  }
 private:
  explicit Implies(
    const Predicate *antecedent,
    const Predicate *consequent) :
    antecedent_(antecedent), consequent_(consequent) { }

  const Predicate *antecedent_;
  const Predicate *consequent_;
};

// A Not boolean expression in a predicate. Just negates the inner predicate.
class Not : public Predicate {
 public:
  static const Not *Create(PredicateArena &arena, const Predicate *negated);
  virtual ~Not() { }

  std::string ToDatalogRuleBody(
      const AccessPath &ap, const DatalogPrintContext &ctxt) const override {
    constexpr absl::string_view kFormatString = R"(isPrincipal(owner), !(%s))";
    return absl::StrFormat(kFormatString,
                           negated_predicate_->ToDatalogRuleBody(ap, ctxt));
  }

  static PredicateKind GetKind() {
    return PredicateKind::kNot;
  }

  PredicateKind GetPredicateKind() const override { return GetKind(); }

  bool operator==(const Predicate &other) const override {
    const Not *other_not = Predicate::DynCast<Not>(other);
    if (other_not == nullptr) { return false; }
    return *negated_predicate_ == *other_not->negated_predicate_;
  }
 private:
  explicit Not(const Predicate *negated_predicate)
    : negated_predicate_(negated_predicate) { }

  const Predicate *negated_predicate_;
};

// A boolean or expression on two predicates.
class Or : public Predicate {
 public:
  static const Or *Create(
      PredicateArena &arena, const Predicate *lhs, const Predicate *rhs);
  virtual ~Or() { }

  std::string ToDatalogRuleBody(
      const AccessPath &ap, const DatalogPrintContext &ctxt) const override {
    constexpr absl::string_view kFormatString = R"(((%s); (%s)))";
    return absl::StrFormat(kFormatString, lhs_->ToDatalogRuleBody(ap, ctxt),
                           rhs_->ToDatalogRuleBody(ap, ctxt));
  }

  static PredicateKind GetKind() {
    return PredicateKind::kOr;
  }

  PredicateKind GetPredicateKind() const override { return GetKind(); }

  bool operator==(const Predicate &other) const override {
    const Or *other_or = Predicate::DynCast<Or>(other);
    if (other_or == nullptr) return false;
    return (*lhs_ == *other_or->lhs_) && (*rhs_ == *other_or->rhs_);
  }
 private:
  explicit Or(const Predicate *lhs, const Predicate *rhs)
    : lhs_(lhs), rhs_(rhs) { }

  const Predicate *lhs_;
  const Predicate *rhs_;
};

// A boolean expression that is true if a tag is possibly present on an
// AccessPath, false if it cannot possibly be present. The leaf of the
// predicate tree.
class TagPresence : public Predicate {
 public:
  static const TagPresence *Create(PredicateArena &arena, std::string tag);
  virtual ~TagPresence() {}

  // The tag presence just turns into a check for the tag on the access_path
  // in the mayHaveTag relation.
  std::string ToDatalogRuleBody(
      const AccessPath &access_path,
      const DatalogPrintContext &ctxt) const override {
    constexpr absl::string_view kFormatStr = R"(mayHaveTag("%s", owner, "%s"))";
    return absl::StrFormat(kFormatStr, access_path.ToDatalog(ctxt), tag_);
  }

  static PredicateKind GetKind() {
    return kTagPresence;
  }

  PredicateKind GetPredicateKind() const override {
    return GetKind();
  }

  bool operator==(const Predicate &other) const override {
    const TagPresence *other_tag_presence =
        Predicate::DynCast<TagPresence>(other) ;
    if (other_tag_presence == nullptr) {
      return false;
    }
    return tag_ == other_tag_presence->tag_;
  }
 private:
  explicit TagPresence(std::string tag) : tag_(std::move(tag)) { }

  std::string tag_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_PREDICATES_PREDICATE_H_
