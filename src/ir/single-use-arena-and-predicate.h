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

#ifndef SRC_IR_SINGLE_USE_ARENA_AND_PREDICATE_H_
#define SRC_IR_SINGLE_USE_ARENA_AND_PREDICATE_H_

#include "src/ir/predicate_arena.h"

namespace raksha::ir {

// This class is used to construct example objects for test parameters
// without introducing mutable global state. It constructs an arena, uses it
// to construct a single predicate, and holds both the constructed arena and
// the predicate. This should only be used for tests.
template<class T, class ...CtorArgs>
class SingleUseArenaAndPredicate {
 public:
  SingleUseArenaAndPredicate(CtorArgs... args)
    : arena_(), predicate_(T::Create(arena_, args...)) { }

  const T *predicate() const { return predicate_; }

 private:
  PredicateArena arena_;
  const T *predicate_;
};

using SingleUseArenaAndTagPresence =
    SingleUseArenaAndPredicate<TagPresence, std::string>;
using SingleUseArenaAndAnd =
    SingleUseArenaAndPredicate<And, const Predicate *, const Predicate *>;
using SingleUseArenaAndImplies =
    SingleUseArenaAndPredicate<Implies, const Predicate *, const Predicate *>;
using SingleUseArenaAndNot = SingleUseArenaAndPredicate<Not, const Predicate *>;
using SingleUseArenaAndOr =
    SingleUseArenaAndPredicate<Or, const Predicate *, const Predicate *>;

}  // namespace raksha::ir

#endif  // SRC_IR_SINGLE_USE_ARENA_AND_PREDICATE_H_
