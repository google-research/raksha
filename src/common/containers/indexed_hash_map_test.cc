#include "src/common/containers/indexed_hash_map.h"

#include "fuzztest/fuzztest.h"
#include "src/common/testing/gtest.h"

namespace raksha::common::containers {
namespace {

template <class Key, class Value>
class IndexedHashMapTestFixture {
 public:
  explicit IndexedHashMapTestFixture(std::vector<Key> keys,
                                     std::vector<Value> values)
      : key_not_in_map_(std::move(*keys.rbegin())) {
    keys.pop_back();
    size_t keys_size = keys.size();
    key_value_pairs_in_map_.reserve(keys_size);
    for (size_t i = 0; i < keys_size; ++i) {
      key_value_pairs_in_map_.push_back(
          std::make_pair(keys.at(i), values.at(i % values.size())));
    }
    map_ = IndexedHashMap<Key, Value>(key_value_pairs_in_map_);
  }
  const Key &key_not_in_map() { return key_not_in_map_; }
  IndexedHashMap<Key, Value> &map() { return map_; }
  const std::vector<std::pair<Key, Value>> &key_value_pairs_in_map() {
    return key_value_pairs_in_map_;
  }

 private:
  Key key_not_in_map_;
  std::vector<std::pair<Key, Value>> key_value_pairs_in_map_;
  IndexedHashMap<Key, Value> map_;
};

template <class Key, class Value>
void TestGetIndex(std::vector<Key> keys, std::vector<Value> values) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  const std::vector<std::pair<Key, Value>> &key_value_pairs_in_map =
      fixture.key_value_pairs_in_map();
  for (size_t idx = 0; idx < key_value_pairs_in_map.size(); ++idx) {
    Key key = key_value_pairs_in_map.at(idx).first;
    EXPECT_EQ(fixture.map().GetIndex(key), idx);
  }
  EXPECT_DEATH({ fixture.map().GetIndex(fixture.key_not_in_map()); },
               "Key not present in map!");
}

constexpr auto TestGetIndexUints = TestGetIndex<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestGetIndexUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1));

constexpr auto TestGetIndexStrings = TestGetIndex<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestGetIndexStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1));

template <class Key, class Value>
void TestGetEntry(std::vector<Key> keys, std::vector<Value> values) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  for (auto &[key, value] : fixture.key_value_pairs_in_map()) {
    EXPECT_EQ(fixture.map().Get(key), value);
  }
  EXPECT_DEATH({ fixture.map().Get(fixture.key_not_in_map()); },
               "Key not present in map!");
}

constexpr auto TestGetEntryUints = TestGetEntry<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestGetEntryUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1));

constexpr auto TestGetEntryStrings = TestGetEntry<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestGetEntryStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1));

template <class Key, class Value>
void TestSize(std::vector<Key> keys, std::vector<Value> values) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  EXPECT_EQ(fixture.map().Size(), fixture.key_value_pairs_in_map().size());
}

constexpr auto TestSizeUints = TestSize<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestSizeUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1));

constexpr auto TestSizeStrings = TestSize<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestSizeStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1));

// Check that the iteration order over values in the map is the same as the
// insertion order.
template <class Key, class Value>
void TestIterationOrder(std::vector<Key> keys, std::vector<Value> values) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  uint64_t idx = 0;
  const IndexedHashMap<Key, Value> &map = fixture.map();
  const std::vector<std::pair<Key, Value>> &key_value_pairs_in_map =
      fixture.key_value_pairs_in_map();
  for (const Value &value : map) {
    EXPECT_EQ(value, key_value_pairs_in_map.at(idx).second);
    ++idx;
  }
}

constexpr auto TestIterationOrderUints = TestIterationOrder<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestIterationOrderUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1));

constexpr auto TestIterationOrderStrings =
    TestIterationOrder<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestIterationOrderStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1));

template <class Key, class Value>
void TestInsertionPreservesOrder(std::vector<Key> keys,
                                 std::vector<Value> values,
                                 Value additional_value) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  IndexedHashMap<Key, Value> &map = fixture.map();
  Key new_key = fixture.key_not_in_map();
  map.InsertExpectSuccess(new_key, additional_value);
  std::vector<std::pair<Key, Value>> expected_key_value_pairs =
      fixture.key_value_pairs_in_map();
  expected_key_value_pairs.push_back(std::make_pair(new_key, additional_value));

  uint64_t idx = 0;
  for (const Value &value : map) {
    EXPECT_EQ(value, expected_key_value_pairs.at(idx).second);
    ++idx;
  }
}

constexpr auto TestInsertionPreservesOrderUints =
    TestInsertionPreservesOrder<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestInsertionPreservesOrderUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1),
        fuzztest::Arbitrary<uint64_t>());

constexpr auto TestInsertionPreservesOrderStrings =
    TestInsertionPreservesOrder<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestInsertionPreservesOrderStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1),
                 fuzztest::AsciiString());

template <class Key, class Value>
void TestInsertIfNotPresent(std::vector<Key> keys, std::vector<Value> values,
                            Value additional_value) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  IndexedHashMap<Key, Value> &map = fixture.map();
  Key new_key = fixture.key_not_in_map();
  EXPECT_TRUE(map.InsertIfNotPresent(new_key, additional_value));
  EXPECT_EQ(map.Get(new_key), additional_value);
}

constexpr auto TestInsertIfNotPresentUints =
    TestInsertIfNotPresent<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestInsertIfNotPresentUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1),
        fuzztest::Arbitrary<uint64_t>());

constexpr auto TestInsertIfNotPresentStrings =
    TestInsertIfNotPresent<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestInsertIfNotPresentStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1),
                 fuzztest::AsciiString());

template <class Key, class Value>
void TestInsertExpectSuccess(std::vector<Key> keys, std::vector<Value> values,
                             Value additional_value) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  IndexedHashMap<Key, Value> &map = fixture.map();
  Key new_key = fixture.key_not_in_map();
  map.InsertExpectSuccess(new_key, additional_value);
  EXPECT_EQ(map.Get(new_key), additional_value);
}

constexpr auto TestInsertExpectSuccessUints =
    TestInsertExpectSuccess<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestInsertExpectSuccessUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(1),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(1),
        fuzztest::Arbitrary<uint64_t>());

constexpr auto TestInsertExpectSuccessStrings =
    TestInsertExpectSuccess<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestInsertExpectSuccessStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(1),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(1),
                 fuzztest::AsciiString());

template <class Key, class Value>
void TestDoubleInsertIfNotPresent(std::vector<Key> keys,
                                  std::vector<Value> values,
                                  size_t index_to_reinsert) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  std::vector<std::pair<Key, Value>> key_value_pairs =
      fixture.key_value_pairs_in_map();
  std::pair<Key, Value> key_value_pair =
      key_value_pairs.at(index_to_reinsert % key_value_pairs.size());
  IndexedHashMap<Key, Value> &map = fixture.map();
  EXPECT_FALSE(
      map.InsertIfNotPresent(key_value_pair.first, key_value_pair.second));
}

constexpr auto TestDoubleInsertIfNotPresentUints =
    TestDoubleInsertIfNotPresent<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestDoubleInsertIfNotPresentUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(2),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(2),
        fuzztest::Arbitrary<size_t>());

constexpr auto TestDoubleInsertIfNotPresentStrings =
    TestDoubleInsertIfNotPresent<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestDoubleInsertIfNotPresentStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(2),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(2),
                 fuzztest::Arbitrary<size_t>());

template <class Key, class Value>
void TestDoubleInsertExpectSuccess(std::vector<Key> keys,
                                   std::vector<Value> values,
                                   size_t index_to_reinsert) {
  IndexedHashMapTestFixture<Key, Value> fixture(keys, values);
  std::vector<std::pair<Key, Value>> key_value_pairs =
      fixture.key_value_pairs_in_map();
  std::pair<Key, Value> key_value_pair =
      key_value_pairs.at(index_to_reinsert % key_value_pairs.size());
  IndexedHashMap<Key, Value> &map = fixture.map();
  EXPECT_DEATH(
      { map.InsertExpectSuccess(key_value_pair.first, key_value_pair.second); },
      "Key was already associated with a value!");
}

constexpr auto TestDoubleInsertExpectSuccessUints =
    TestDoubleInsertExpectSuccess<uint64_t, uint64_t>;
FUZZ_TEST(IndexedHashMapTest, TestDoubleInsertExpectSuccessUints)
    .WithDomains(
        fuzztest::UniqueElementsVectorOf(fuzztest::Arbitrary<uint64_t>())
            .WithMinSize(2),
        fuzztest::VectorOf(fuzztest::Arbitrary<uint64_t>()).WithMinSize(2),
        fuzztest::Arbitrary<size_t>());

constexpr auto TestDoubleInsertExpectSuccessStrings =
    TestDoubleInsertExpectSuccess<std::string, std::string>;
FUZZ_TEST(IndexedHashMapTest, TestDoubleInsertExpectSuccessStrings)
    .WithDomains(fuzztest::UniqueElementsVectorOf(fuzztest::AsciiString())
                     .WithMinSize(2),
                 fuzztest::VectorOf(fuzztest::AsciiString()).WithMinSize(2),
                 fuzztest::Arbitrary<size_t>());

}  // namespace
}  // namespace raksha::common::containers
