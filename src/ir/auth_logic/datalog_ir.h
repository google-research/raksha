/*
 * Copyright 2022 Google LLC.
 *
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
#ifndef SRC_IR_AUTH_LOGIC_DATALOG_IR_H_
#define SRC_IR_AUTH_LOGIC_DATALOG_IR_H_

#include <vector>

#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

// A conditional datalog assertion with a left hand side and a right hand
// side.
class DLIRCondAssertion {
 public:
  explicit DLIRCondAssertion(Predicate lhs, std::vector<Predicate> rhs)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
  const Predicate& lhs() const { return lhs_; }
  const std::vector<Predicate>& rhs() const { return rhs_; }

 private:
  Predicate lhs_;
  std::vector<Predicate> rhs_;
};

// A Datalog IR assertion is either:
//  - an unconditional fact which is just a predicate
//  - a conditional assertion
class DLIRAssertion {
 public:
  // DLIRAssertionVariantType represents the alternative forms for
  // DLIRAssertions. Client code should use this type for traversing
  // DLIRAssertions. This type may be changed in the future.
  using DLIRAssertionVariantType = std::variant<Predicate, DLIRCondAssertion>;
  explicit DLIRAssertion(DLIRAssertionVariantType value)
      : value_(std::move(value)) {}
  const DLIRAssertionVariantType& GetValue() const { return value_; }

 private:
  std::variant<Predicate, DLIRCondAssertion> value_;
};

class DLIRProgram {
 public:
  DLIRProgram(std::vector<DLIRAssertion> assertions,
              std::vector<std::string> outputs)
      : assertions_(std::move(assertions)), outputs_(std::move(outputs)) {}
  const std::vector<DLIRAssertion>& assertions() const { return assertions_; }
  const std::vector<std::string>& outputs() const { return outputs_; }

 private:
  std::vector<DLIRAssertion> assertions_;
  std::vector<std::string> outputs_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_DATALOG_IR_H_
