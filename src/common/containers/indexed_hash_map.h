#ifndef SRC_COMMON_CONTAINERS_INDEXED_HASH_MAP_H_
#define SRC_COMMON_CONTAINERS_INDEXED_HASH_MAP_H_

#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/log/check.h"

namespace raksha::common::containers {

// A hash map with constant-time lookup, but which preserves the order of
// elements from insertion. It does this by being actually a regular hash map
// combined with a vector, where the map goes from key to vector index, where
// the vector contains the map's values.
//
// This provides constant time lookup and stable iteration order matching the
// order in which elements were inserted, at the cost of about 1 size_t of
// memory per key-value pair plus a non-contiguous memory access from the map to
// the vector.
//
// In addition, because the layout is relatively odd, it would be difficult to
// use the traditional C++ map interface. Instead, we use something a little
// more akin to the interface you'd see for a Java map.
template <class Key, class Value>
class IndexedHashMap {
 public:
  explicit IndexedHashMap<Key, Value>() = default;
  explicit IndexedHashMap(
      std::vector<std::pair<Key, Value>> key_value_pair_vector) {
    for (auto iter = std::make_move_iterator(key_value_pair_vector.begin());
         iter != std::make_move_iterator(key_value_pair_vector.end()); ++iter) {
      auto [key, value] = std::move(*iter);
      InsertExpectSuccess(std::move(key), std::move(value));
    }
  }

  class iterator {
   public:
    using InnerIterator = typename std::vector<Value>::const_iterator;
    iterator(InnerIterator inner_iterator)
        : inner_iterator_(std::move(inner_iterator)) {}

    iterator operator++() {
      ++inner_iterator_;
      return *this;
    }

    iterator operator++(int) {
      iterator previous = *this;
      ++(*this);
      return previous;
    }

    bool operator==(const iterator &other) const {
      return inner_iterator_ == other.inner_iterator_;
    }

    bool operator!=(const iterator &other) const { return !(*this == other); }

    const Value &operator*() const { return *inner_iterator_; }

   private:
    friend class IndexedHashMap<Key, Value>;
    InnerIterator inner_iterator_;
  };

  iterator begin() const { return iterator(value_vector_.begin()); }

  iterator end() const { return iterator(value_vector_.end()); }

  // Associate `value` with `key` if `key` is not yet associated with any value.
  // If it is already associated with a value, do nothing. Return whether or not
  // the map was successfully modified.
  bool InsertIfNotPresent(Key key, Value value) {
    auto [iterator, insert_success] =
        key_to_index_map_.insert({std::move(key), value_vector_.size()});
    if (insert_success) {
      value_vector_.push_back(std::move(value));
    }
    return insert_success;
  }

  void InsertExpectSuccess(Key key, Value value) {
    CHECK(InsertIfNotPresent(std::move(key), std::move(value)))
        << "Key was already associated with a value!";
  }

  size_t GetIndex(Key key) const {
    auto find_result = key_to_index_map_.find(key);
    CHECK(find_result != key_to_index_map_.end()) << "Key not present in map!";
    return find_result->second;
  }

  const Value &Get(Key key) const { return value_vector_.at(GetIndex(key)); }

  size_t Size() const { return key_to_index_map_.size(); }

 private:
  absl::flat_hash_map<Key, size_t> key_to_index_map_;
  std::vector<Value> value_vector_;
};
}  // namespace raksha::common::containers

#endif  // SRC_COMMON_CONTAINERS_INDEXED_HASH_MAP_H_
