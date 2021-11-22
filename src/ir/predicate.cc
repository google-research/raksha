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

#include "src/ir/predicate.h"

namespace raksha::ir {

std::unique_ptr<And> And::Create(std::unique_ptr<Predicate> lhs,
                                 std::unique_ptr<Predicate> rhs) {
  return std::unique_ptr<And>(new And(std::move(lhs), std::move(rhs)));
}

std::unique_ptr<Implies> Implies::Create(
    std::unique_ptr<Predicate> antecedent,
    std::unique_ptr<Predicate> consequent) {
  return std::unique_ptr<Implies>(
      new Implies(std::move(antecedent), std::move(consequent)));
}

std::unique_ptr<Not> Not::Create(std::unique_ptr<Predicate> negated) {
  return std::unique_ptr<Not>(new Not(std::move(negated)));
}

std::unique_ptr<Or> Or::Create(std::unique_ptr<Predicate> lhs,
                               std::unique_ptr<Predicate> rhs) {
  return std::unique_ptr<Or>(new Or(std::move(lhs), std::move(rhs)));
}

std::unique_ptr<TagPresence> TagPresence::Create(std::string tag) {
  return std::unique_ptr<TagPresence>(new TagPresence(std::move(tag)));
}

}  // namespace raksha::ir
