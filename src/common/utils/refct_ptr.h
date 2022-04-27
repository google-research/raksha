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

#ifndef SRC_COMMON_UTILS_REFCT_PTR_H_
#define SRC_COMMON_UTILS_REFCT_PTR_H_

#include "src/common/utils/intrusive_ptr.h"
#include "src/common/utils/ref_counted.h"

namespace raksha::common::utils {

template <class T>
class WithRefCount : public RefCounted<WithRefCount<T>> {
 public:
  template <class... Args>
  WithRefCount(Args&&... args)
      : RefCounted<WithRefCount<T>>(), payload_(std::forward<Args>(args)...) {}

  T& payload() { return payload_; }
  const T& payload() const { return payload_; }

 private:
  T payload_;
};

// An intrusive_ptr that only intrudes on the `WithRefCount` object it
// introduces. As such, it's not really "intrusive" from the point of view of
// the client and deserves a new name.
template <class T>
class refct_ptr {
 public:
  refct_ptr() : ptr_() {}
  refct_ptr(intrusive_ptr<WithRefCount<T>> ptr) : ptr_(std::move(ptr)) {}

  T& operator*() { return (*ptr_).payload(); }

  const T& operator*() const { return (*ptr_).payload(); }

  T* operator->() { return &(*ptr_).payload(); }

  const T* operator->() const { return &(*ptr_).payload(); }

  explicit operator bool() const { return bool(ptr_); }

  intrusive_ptr<WithRefCount<T>> get_intrusive_ptr() const { return ptr_; }

 private:
  intrusive_ptr<WithRefCount<T>> ptr_;
};

template <class T, class... Args>
refct_ptr<T> make_refct_ptr(Args&&... args) {
  return intrusive_ptr<WithRefCount<T>>(
      new WithRefCount<T>(std::forward<Args>(args)...));
}

}  // namespace raksha::common::utils

#endif  // SRC_COMMON_UTILS_REFCT_PTR_H_
