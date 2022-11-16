#ifndef SRC_COMMON_UTILS_RANGES_H_
#define SRC_COMMON_UTILS_RANGES_H_

#include "src/common/utils/iterator_adapter.h"
#include "src/common/utils/iterator_range.h"

namespace raksha::utils::ranges {

namespace internal {
// Value adapters for maps.

// Picks and returns the key of a map element.
template <typename C>
struct MapKeySelector {
  const typename C::key_type& operator()(
      const typename C::value_type& value) const {
    return value.first;
  }
};

// Picks and returns the value of a map element.
template <typename C>
struct MapValueSelector {
  const typename C::mapped_type& operator()(
      const typename C::value_type& value) const {
    return value.second;
  }
};

}  // namespace internal

// Returns a range to iterate over all elements in the given container.
template <typename C>
auto all(const C& c) {
  return make_range(std::move(c.begin()), std::move(c.end()));
}

// Returns a range to iterate over all keys in the given map container.
template <typename C>
auto keys(const C& c) {
  return make_adapted_range<internal::MapKeySelector<C>>(c);
}

// Returns a range to iterate over all values in the given map container.
template <typename C>
auto values(const C& c) {
  return make_adapted_range<internal::MapValueSelector<C>>(c);
}

}  // namespace raksha::utils::ranges

#endif  // SRC_COMMON_UTILS_RANGES_H_
