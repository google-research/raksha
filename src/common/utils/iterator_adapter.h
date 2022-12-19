#ifndef SRC_COMMON_UTILS_ITERATOR_ADAPTER_H_
#define SRC_COMMON_UTILS_ITERATOR_ADAPTER_H_

#include <type_traits>

#include "src/common/utils/iterator_range.h"
#include "src/common/utils/types.h"

namespace raksha::utils {

// A class to adapt an iterator. Currently, the class provides a mechanism to
// convert iterator::value_type to another type. In future, we could add
// support for changing the iteration behavior (e.g., filtering).
//
// The `ValueAdapter` should be a functor with the following signature:
//     NewValueType ValueAdapter(const iterator::value_type&, const Context&);
// or  NewValueType ValueAdapter(const iterator::value_type&); // Context = Unit
//
template <typename Iterator, typename ValueAdapter, typename Context = Unit>
struct IteratorAdapter {
  using iterator_category = typename Iterator::iterator_category;

  // Compute value_type for the iterator.
  //
  // If the Context is Unit: the value_type is the result of
  //      ValueAdapter()(const Iterator::value_type&)
  // Otherwise, it is the result of
  //      ValueAdapter()(const Iterator::value_type&, const Context&)
  template <typename IValueAdapter, typename IContext>
  struct ValueAdapterReturnType {
    using type = std::remove_reference_t<typename std::invoke_result<
        IValueAdapter, const typename Iterator::value_type&,
        const IContext&>::type>;
  };
  template <typename IValueAdapter>
  struct ValueAdapterReturnType<IValueAdapter, Unit> {
    using type = std::remove_reference_t<typename std::invoke_result<
        IValueAdapter, const typename Iterator::value_type&>::type>;
  };
  using value_type =
      typename ValueAdapterReturnType<ValueAdapter, Context>::type;
  using difference_type = typename Iterator::difference_type;
  using pointer = value_type*;
  using reference = value_type&;

  IteratorAdapter& operator++() {
    underlying_iterator_++;
    return *this;
  }

  IteratorAdapter operator++(int) {
    IteratorAdapter retval = *this;
    ++(*this);
    return retval;
  }

  value_type operator*() const {
    if constexpr (std::is_same_v<Context, Unit>) {
      return ValueAdapter()(*underlying_iterator_);
    } else {
      return ValueAdapter()(*underlying_iterator_, context_);
    }
  }

 private:
  explicit IteratorAdapter(Iterator underlying_iterator,
                           Context context = Context())
      : underlying_iterator_(std::move(underlying_iterator)),
        context_(std::move(context)) {}

  Iterator underlying_iterator_;
  Context context_;

  template <typename I, typename V, typename C>
  friend bool operator!=(const IteratorAdapter<I, V, C>& lhs,
                         const IteratorAdapter<I, V, C>& rhs);

  template <typename I, typename V, typename C>
  friend bool operator==(const IteratorAdapter<I, V, C>& lhs,
                         const IteratorAdapter<I, V, C>& rhs);

  template <typename A, typename I, typename C>
  friend iterator_range<IteratorAdapter<I, A, C>> make_adapted_range(I begin,
                                                                     I end,
                                                                     C context);
};

template <typename Iterator, typename ValueAdapter, typename Context>
inline bool operator==(
    const IteratorAdapter<Iterator, ValueAdapter, Context>& lhs,
    const IteratorAdapter<Iterator, ValueAdapter, Context>& rhs) {
  return lhs.underlying_iterator_ == rhs.underlying_iterator_ &&
         lhs.context_ == rhs.context_;
}

template <typename Iterator, typename ValueAdapter, typename Context>
inline bool operator!=(
    const IteratorAdapter<Iterator, ValueAdapter, Context>& lhs,
    const IteratorAdapter<Iterator, ValueAdapter, Context>& rhs) {
  return !(lhs == rhs);
}

// Wraps the iterators in an adapter along with the given context and returns
// the range.
//
// Example:
//     struct ValueAdapter {
//       T operator()(const Iterator::value_type& v, const Context& c) { ... }
//     }
//     Container c;
//     auto range = make_adapted_range<ValueAdapter>(c.begin(), c.end(), c);
//
template <typename ValueAdapter, typename Iterator, typename Context>
iterator_range<IteratorAdapter<Iterator, ValueAdapter, Context>>
make_adapted_range(Iterator begin, Iterator end, Context context) {
  return utils::make_range(IteratorAdapter<Iterator, ValueAdapter, Context>(
                               std::move(begin), context),
                           IteratorAdapter<Iterator, ValueAdapter, Context>(
                               std::move(end), context));
}

// Wraps the iterators in an adapter (no context) and returns the range.
//
// Example:
//     struct ValueAdapter {
//       T operator()(const Iterator::value_type& v) { ... }
//     }
//     Container c;
//     auto range = make_adapted_range<ValueAdapter>(c.begin(), c.end());
//
template <typename ValueAdapter, typename Iterator>
iterator_range<IteratorAdapter<Iterator, ValueAdapter, Unit>>
make_adapted_range(Iterator begin, Iterator end) {
  return make_adapted_range<ValueAdapter>(std::move(begin), std::move(end),
                                          Unit());
}

// Wraps the begin() and end() iterators of the container in an adapter along
// with the given context and returns the range.
//
// Example:
//     struct ValueAdapter {
//       T operator()(const Iterator::value_type& v, const Context& c) { ... }
//     }
//     Container c;
//     auto range = make_adapted_range<ValueAdapter>(c, context);
//
template <typename ValueAdapter, typename Container, typename Context>
auto make_adapted_range(const Container& c, Context context) {
  return make_adapted_range<ValueAdapter>(
      std::move(c.begin()), std::move(c.end()), std::move(context));
}

// Wraps the begin() and end() iterators of the container in an adapter (no
// context) and returns the range.
//
// Example:
//     struct ValueAdapter {
//       T operator()(const Iterator::value_type& v) { ... }
//     }
//     Container c;
//     auto range = make_adapted_range<ValueAdapter>(c);
//
template <typename ValueAdapter, typename Container>
auto make_adapted_range(const Container& c) {
  return make_adapted_range<ValueAdapter>(c, Unit());
}

}  // namespace raksha::utils

#endif  // SRC_COMMON_UTILS_ITERATOR_ADAPTER_H_
