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

#ifndef SRC_IR_FAKE_PREDICATE_ARENA_H_
#define SRC_IR_FAKE_PREDICATE_ARENA_H_

#include "src/ir/predicate_arena.h"

namespace raksha::ir {

// An "arena" that just returns the objects it should capture, releasing them
// from the std::unique_ptr, which leaks them. Allows avoiding mutable global
// state in tests. This should never be used in production.
class FakePredicateArenaImpl : public PredicateArena {
 public:
  virtual ~FakePredicateArenaImpl() {}
  const Predicate *CapturePredicateBase(
      std::unique_ptr<Predicate> pred) override {
    return pred.release();
  }
};

}  // namespace raksha::ir

#endif  // SRC_IR_FAKE_PREDICATE_ARENA_H_
