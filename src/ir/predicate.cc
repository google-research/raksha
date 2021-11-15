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
#include "src/ir/predicate_arena.h"

namespace raksha::ir {

const And *And::Create(
    PredicateArena &arena, const Predicate *lhs, const Predicate *rhs) {
  return arena.CapturePredicate(std::unique_ptr<And>(new And(lhs, rhs)));
}

const Implies *Implies::Create(
    PredicateArena &arena,
    const Predicate *antecedent,
    const Predicate *consequent) {
  return arena.CapturePredicate(std::unique_ptr<Implies>(
      new Implies(antecedent, consequent)));
}

const Not *Not::Create(PredicateArena &arena, const Predicate *negated) {
  return arena.CapturePredicate(std::unique_ptr<Not>(new Not(negated)));
}

const Or *Or::Create(PredicateArena &arena,
    const Predicate *lhs, const Predicate *rhs) {
  return arena.CapturePredicate(std::unique_ptr<Or>(new Or(lhs, rhs)));
}

const TagPresence *TagPresence::Create(PredicateArena &arena, std::string tag) {
  return arena.CapturePredicate(std::unique_ptr<TagPresence>(
      new TagPresence(std::move(tag))));
}

}  // namespace raksha::ir
