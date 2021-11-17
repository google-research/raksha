#ifndef SRC_IR_ACCESS_PATH_SELECTORS_SET_H_
#define SRC_IR_ACCESS_PATH_SELECTORS_SET_H_

#include "access_path_selectors.h"

#include "absl/container/flat_hash_set.h"
#include "absl/container/flat_hash_map.h"

namespace raksha::ir {

// This class implements a set of AccessPathSelectors. It does not actually
// contain a set; it contains a vector. This makes mutating the "set" much
// easier, as we add parent paths on as this is passed upwards. We expect
// that invariants on the type tree will make most additions to the set
// unique anyway. We lazily unique the entries when it matters.
class AccessPathSelectorsSet {
 public:
  // The default constructor will construct an empty set.
  AccessPathSelectorsSet() = default;

  // Allow creating an AccessPathSelectorSet with an explicit list of items
  // that should be in that set.
  explicit AccessPathSelectorsSet(
      absl::flat_hash_map<Selector, std::unique_ptr<AccessPathSelectorTree>>
      contents)
    : selector_trees_(std::move(contents)) {}

  // Returns a set that is the union of the two passed-in sets.
  static AccessPathSelectorsSet Union(
      AccessPathSelectorsSet set1, AccessPathSelectorsSet set2) {
    uint64_t set1_size = set1.selector_trees_.size();
    uint64_t set2_size = set2.selector_trees_.size();
    set1.selector_trees_.insert(
        std::make_move_iterator(set2.selector_trees_.begin()),
        std::make_move_iterator(set2.selector_trees_.end()));
    // If the size is not the sum of the sets pre-insertion, then there was a
    // duplicate key.
    CHECK(set1.selector_trees_.size() == (set1_size + set2_size))
      << "Found duplicate selector when unioning two AccessPathSelectorsSets.";
    return set1;
  }

  // Returns a set that has the same elements as child_set but with
  // parent_selector added as a parent to each of them.
  static AccessPathSelectorsSet AddParentToAll(
      Selector parent_selector, AccessPathSelectorsSet child_set) {
    return AccessPathSelectorsSet(
        { std::make_unique<AccessPathSelectorTree>(
            std::move(parent_selector),
            std::move(child_set.selector_trees_)) } );
  }

  // Move the contents of the given AccessPathSelectorsSet into a new
  // absl::flat_hash_set. This uniques and provides access to the contents of
  // this "set".
  static absl::flat_hash_set<AccessPathSelectors> CreateAbslSet(
      AccessPathSelectorsSet original_set) {
    return absl::flat_hash_set<AccessPathSelectors>(
        std::make_move_iterator(original_set.inner_vec_.begin()),
        std::make_move_iterator(original_set.inner_vec_.end()));
  }

 private:
  // The map from selectors to subtrees. The map is used not for lookup, but
  // to guarantee that no two subtrees start with the same selector.
  absl::flat_hash_map<
      Selector, std::unique_ptr<AccessPathSelectorTree>> selector_trees_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_SELECTORS_SET_H_
