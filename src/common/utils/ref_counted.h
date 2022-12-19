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
#ifndef SRC_COMMON_UTILS_REF_COUNTED_H_
#define SRC_COMMON_UTILS_REF_COUNTED_H_

#include <iostream>

#include "src/common/utils/ref_counter.h"

namespace raksha {

template <typename T>
class intrusive_ptr;

// Specialize this class for your type if you want custom reference counting.
template <typename T>
class RefCountManager {
  static void Retain(T* ptr) { ptr->Retain(); }
  static void Release(T* ptr) { ptr->Release(); }

  // So that it can call `Retain` and `Release`.
  friend class intrusive_ptr<T>;
};

// A helper class that allows a type `T` to be reference counted for an
// `intrusive_ptr`.
template <typename T>
class RefCounted {
 protected:
  RefCounted() : count_(0) {}

  // Disable copies and moves of RefCounted objects because they should be done
  // by classes like intrusive_ptr, which update the reference counts
  // appropriately.
  RefCounted(const RefCounted& other) = delete;
  RefCounted(const RefCounted&& other) = delete;
  RefCounted& operator=(const RefCounted&) = delete;
  RefCounted& operator=(RefCounted&&) = delete;

 private:
  void Retain() const { count_.FetchAdd(1); }

  void Release() const {
    size_t old = count_.FetchSub(1);
    if (old == 1) {
      delete this;
    }
  }

 protected:
  virtual ~RefCounted() {}

 private:
  // So that it can call `Retain` and `Release`.
  template <typename U>
  friend class RefCountManager;

  mutable RefCounter count_;
};

}  // namespace raksha

#endif  // SRC_COMMON_UTILS_REF_COUNTED_H_
