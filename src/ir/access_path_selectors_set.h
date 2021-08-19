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
  // Returns a set that is the union of the two passed-in sets.
  static AccessPathSelectorsSet Union(
      AccessPathSelectorsSet set1, AccessPathSelectorsSet set2);

  // Returns a set that is the intersection of the two passed-in sets.
  static AccessPathSelectorsSet Intersect(
      AccessPathSelectorsSet set1, AccessPathSelectorsSet set2);

  // Returns a set that has the same elements as child_set but with
  // parent_selector added as a parent to each of them.
  static AccessPathSelectorsSet AddParentToAll(
      Selector parent_selector, AccessPathSelectorsSet child_set);

  // Add a single access_path to the set.
  void Add(AccessPathSelectors access_path) {
    inner_vec_.push_back(std::move(access_path));
  }

  // Add all entries from the other set to this set.
  void AddAll(AccessPathSelectorsSet other);

  // Return if the set is empty.
  bool IsEmpty() const {
    return inner_vec_.empty();
  }

  // Move the contents of this AccessPathSelectorsSet into a new
  // absl::flat_hash_set. This uniques and provides access to the contents of
  // this "set".
  absl::flat_hash_set<AccessPathSelectors> MoveIntoAbslSet();

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
