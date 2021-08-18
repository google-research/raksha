#ifndef SRC_UTIL_COPYABLE_H_
#define SRC_UTIL_COPYABLE_H_

#include <utility>

namespace raksha::util {

template<class T>
class Copyable {
 public:
  explicit Copyable(T inner) : inner_(std::move(inner)) {}

  Copyable(const Copyable &other) :
    inner_(other.inner_.Copy())
  {}

  // Allow an implicit cast to the wrapped type to automatically drop the
  // implicit copyability.
  operator T() {
    return inner_.Copy();
  }

  // Implement the operators that allow Copyable to act like a smart pointer
  // to inner_.
  T& operator*() {
    return inner_;
  }

  const T& operator*() const {
    return inner_;
  }

  T* operator->() {
    return &inner_;
  }

  const T* operator->() const {
    return &inner_;
  }

 private:
  T inner_;
};

}  // namespace src::util

#endif  // SRC_UTIL_COPYABLE_H_
