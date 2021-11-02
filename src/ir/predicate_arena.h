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

#ifndef SRC_IR_PREDICATE_ARENA_H_
#define SRC_IR_PREDICATE_ARENA_H_

#include <memory>
#include <vector>

#include "src/ir/predicate.h"

namespace raksha::ir {

class PredicateArena {
  public:
  virtual ~PredicateArena() {}

  // CapturePredicate takes in an owning pointer for some predicate base
  // class, takes ownership of it, and returns a pointer to that same
  // predicate base class. This
  template<class T>
  const T *CapturePredicate(std::unique_ptr<T> pred) {
    return static_cast<const T *>(CapturePredicateBase(std::move(pred)));
  }

 protected:
  virtual const Predicate *CapturePredicateBase(
      std::unique_ptr<Predicate> pred) = 0;
};

// An arena that constructs and holds predicate objects. This is used to
// ensure that all predicates live long enough to be printed out to Datalog.
// This is the one version of the PredicateArena that we expect to use in
// non-testing code.
class PredicateArenaImpl : public PredicateArena {
 public:
  virtual ~PredicateArenaImpl() {}

 protected:
  const Predicate *CapturePredicateBase(
      std::unique_ptr<Predicate> pred) override {
    owned_predicates_.push_back(std::move(pred));
    return owned_predicates_.back().get();
  }
 private:
  std::vector<std::unique_ptr<Predicate>> owned_predicates_;

};

}  // namespace raksha::ir

#endif  // SRC_IR_PREDICATE_ARENA_H_
