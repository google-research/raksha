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
//-----------------------------------------------------------------------------
#ifndef SRC_UTILS_REF_COUNTER_H_
#define SRC_UTILS_REF_COUNTER_H_

#include <cstddef>

#include "src/common/logging/logging.h"

namespace raksha {

// Note that this is not a thread-safe class.
class RefCounter {
 public:
  RefCounter() : count_(0) {}
  RefCounter(size_t init) : count_(init) {}

  operator size_t() const { return count_; }

  // Adds the given increment and returns the old value.
  size_t FetchAdd(size_t increment) {
    size_t old = count_;
    count_ += increment;
    return old;
  }

  // Subtracts the given increment and returns the old value.
  size_t FetchSub(size_t decrement) {
    CHECK(count_ >= decrement) << "FetchSub can decrement below zero.";
    size_t old = count_;
    count_ -= decrement;
    return old;
  }

 private:
  size_t count_;
};

}  // namespace raksha

#endif  // SRC_UTILS_REF_COUNTER_H_
