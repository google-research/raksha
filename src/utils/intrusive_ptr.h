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
#ifndef SRC_UTILS_INTRUSIVE_PTR_H_
#define SRC_UTILS_INTRUSIVE_PTR_H_

#include "src/utils/ref_counted.h"

namespace raksha {

template <typename T>
class intrusive_ptr {
 public:
  intrusive_ptr() : ptr_(nullptr) {}
  intrusive_ptr(T* ptr) : ptr_(Retain(ptr)) {}
  intrusive_ptr(const intrusive_ptr<T>& other) : ptr_(Retain(other.ptr_)) {}
  // There is no need to adjust count on a move.
  intrusive_ptr(intrusive_ptr<T>&& other) : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }
  ~intrusive_ptr() { Release(ptr_); }

  intrusive_ptr<T>& operator=(intrusive_ptr<T> other) {
    // We are using the copy-and-swap idiom for assignment.
    // Argument `other` should be passed by value.
    this->Swap(other);
    return *this;
  }

  T& operator*() const {
    CHECK(ptr_ != nullptr) << "Dereferencing a nullptr!";
    return *ptr_;
  }

  T* operator->() const { return ptr_; }
  T* get() const { return ptr_; }

 private:
  void Swap(intrusive_ptr<T>& other) {
    T* tmp = other.ptr_;
    other.ptr_ = ptr_;
    ptr_ = tmp;
  }

  T* Retain(T* p) {
    if (p) {
      RefCountManager<T>::Retain(p);
    }
    return p;
  }

  void Release(T* p) {
    if (p) {
      RefCountManager<T>::Release(p);
    }
  }

  T* ptr_;
};

// Factory function for creating instances of intrusive_ptr.
template <typename T, typename... Args>
intrusive_ptr<T> make_intrusive_ptr(Args&&... A) {
  return intrusive_ptr<T>(new T(std::forward<Args>(A)...));
}

}  // namespace raksha

#endif  // SRC_UTILS_INTRUSIVE_PTR_H_
