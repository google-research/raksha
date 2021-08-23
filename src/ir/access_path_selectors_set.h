#ifndef SRC_IR_ACCESS_PATH_SELECTORS_SET_H_
#define SRC_IR_ACCESS_PATH_SELECTORS_SET_H_

#include "access_path_selectors.h"

#include "absl/container/flat_hash_set.h"

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
  explicit AccessPathSelectorsSet(std::vector<AccessPathSelectors> contents)
    : inner_vec_(std::move(contents)) {}

  // Returns a set that is the union of the two passed-in sets.
  static AccessPathSelectorsSet Union(
      AccessPathSelectorsSet set1, AccessPathSelectorsSet set2) {
    set1.inner_vec_.insert(
        set1.inner_vec_.end(),
        std::make_move_iterator(set2.inner_vec_.begin()),
        std::make_move_iterator(set2.inner_vec_.end()));
    return set1;
  }

  // Returns a set that is the intersection of the two passed-in sets.
  static AccessPathSelectorsSet Intersect(
      AccessPathSelectorsSet set1, AccessPathSelectorsSet set2) {
    AccessPathSelectorsSet result;

    // Make the first set into a hash set for efficient lookup.
    absl::flat_hash_set<AccessPathSelectors> hash_set1 =
      AccessPathSelectorsSet::CreateAbslSet(std::move(set1));

    // Place items from the second set into the result only if they were in
    // the first set.
    for (AccessPathSelectors &path : set2.inner_vec_) {
      if (hash_set1.contains(path)) {
        result.inner_vec_.push_back(std::move(path));
      }
    }

    return result;
  }

  // Returns a set that has the same elements as child_set but with
  // parent_selector added as a parent to each of them.
  static AccessPathSelectorsSet AddParentToAll(
      Selector parent_selector, AccessPathSelectorsSet child_set) {
    AccessPathSelectorsSet result;

    for (AccessPathSelectors &path : child_set.inner_vec_) {
      result.inner_vec_.push_back(
          AccessPathSelectors(parent_selector, std::move(path)));
    }

    return result;
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
  // The inner vector implementing the set. It is not guaranteed that the
  // contents of this vector are unique (although, due to the invariants of
  // the type tree, we suspect they may well be). It is not guaranteed (and
  // not expected) that the contents of this vector are sorted. We can get
  // away with this only because we do not allow access to the contents of
  // this set until it is requested that we turn it into a flat_hash_set.
  std::vector<AccessPathSelectors> inner_vec_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_SELECTORS_SET_H_
