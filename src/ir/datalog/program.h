/* Copyright 2022 Google LLC.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.

#ifndef SRC_IR_DATALOG_PROGRAM_H_
#define SRC_IR_DATALOG_PROGRAM_H_

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace raksha::ir::datalog {

// Used to represent whether a predicate is negated or not
enum Sign { kNegated, kPositive };

// Predicate corresponds to a predicate of the form
// <pred_name>(arg_1, ..., arg_n), and it may or may not be negated
class Predicate {
 public:
  Predicate(std::string name, std::vector<std::string> args, Sign sign)
      : name_(std::move(name)),
        args_(std::move(args)),
        // TODO move surpresses an unused private field error about sign_
        // for now, get rid of this eventually
        sign_(std::move(sign)) {}

  const std::string& name() const { return name_; }
  const std::vector<std::string>& args() const { return args_; }
  Sign sign() const { return sign_; }

  template <typename H>
  friend H AbslHashValue(H h, const Predicate& p) {
    return H::combine(std::move(h), p.name(), p.args(), p.sign());
  }
  // Equality is also needed to use a Predicate in a flat_hash_set
  bool operator==(const Predicate& otherPredicate) const {
    return this->name() == otherPredicate.name() &&
           this->sign() == otherPredicate.sign() &&
           this->args() == otherPredicate.args();
  }

  // < operator is needed for btree_set, which is only used for declarations.
  // Since declarations are uniquely defined by the name of the predicate,
  // this implementation that just uses < on the predicate names should be
  // sufficent in the context where it is used.
  bool operator<(const Predicate& otherPredicate) const {
    return this->name() < otherPredicate.name();
  }

 private:
  std::string name_;
  std::vector<std::string> args_;
  Sign sign_;
};

// A conditional datalog assertion with a left hand side and/or a right hand
// side. A Rule is either:
//    - an unconditional fact which is a predicate
//    - a conditional assertion
class Rule {
 public:
  explicit Rule(Predicate lhs, std::vector<Predicate> rhs)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
  explicit Rule(Predicate lhs) : lhs_(std::move(lhs)) {}
  const Predicate& lhs() const { return lhs_; }
  const std::vector<Predicate>& rhs() const { return rhs_; }

 private:
  Predicate lhs_;
  std::vector<Predicate> rhs_;
};

class Program {
 public:
  Program(std::vector<Rule> rules,
              std::vector<std::string> outputs)
      : rules_(std::move(rules)), outputs_(std::move(outputs)) {}
  const std::vector<Rule>& rules() const { return rules_; }
  const std::vector<std::string>& outputs() const { return outputs_; }

 private:
  std::vector<Rule> rules_;
  std::vector<std::string> outputs_;
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_PROGRAM_H_
