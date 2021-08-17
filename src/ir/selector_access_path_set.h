#ifndef SRC_IR_SELECTOR_ACCESS_PATH_SET_H_
#define SRC_IR_SELECTOR_ACCESS_PATH_SET_H_

#include "selector_access_path.h"

#include "absl/container/flat_hash_set.h"

namespace raksha::ir {

// This class implements a set of SelectorAccessPaths. It does not actually
// contain a set; it contains a vector. This makes mutating the "set" much
// easier, as we add parent paths on as this is passed upwards. We expect
// that invariants on the type tree will make most additions to the set
// unique anyway. We lazily unique the entries when it matters.
class SelectorAccessPathSet {
 public:
  // Allow the default constructor to just construct an empty set.
  SelectorAccessPathSet() = default;

  // Because these are variable-size containers and we are returning them up
  // a tree, we want to disallow implicitly-requested copies and allow moves
  // to protect against accidentally copying our way to quadratic runtime
  // performance.
  SelectorAccessPathSet(SelectorAccessPathSet &&) = default;
  SelectorAccessPathSet& operator=(SelectorAccessPathSet &&) = default;

  // Returns a set that is the union of the two passed-in sets.
  static SelectorAccessPathSet Union(
      SelectorAccessPathSet set1, SelectorAccessPathSet set2);

  // Returns a set that is the intersection of the two passed-in sets.
  static SelectorAccessPathSet Intersect(
      SelectorAccessPathSet set1, SelectorAccessPathSet set2);

  // Returns a set that has the same elements as child_set but with
  // parent_selector added as a parent to each of them.
  static SelectorAccessPathSet AddParentToAll(
    Selector parent_selector, SelectorAccessPathSet child_set);

  // Add a single access_path to the set.
  void Add(SelectorAccessPath access_path) {
    inner_vec_.push_back(std::move(access_path));
  }

  // Add all entries from the other set to this set.
  void AddAll(const SelectorAccessPathSet other);

  // Return if the set is empty.
  bool IsEmpty() const {
    return inner_vec_.empty();
  }

  // Function explicitly requesting that the set be copied.
  SelectorAccessPathSet Copy() const {
    return SelectorAccessPathSet(*this);
  }

  // Move the contents of this SelectorAccessPathSet into a new
  // absl::flat_hash_set. This uniques and provides access to the contents of
  // this "set".
  absl::flat_hash_set<SelectorAccessPath> MoveIntoAbslSet();

 private:
  // Make the copy constructor explicit and private to prevent silent copies.
  explicit SelectorAccessPathSet(const SelectorAccessPathSet &);

  // The inner vector implementing the set. It is not guaranteed that the
  // contents of this vector are unique (although, due to the invariants of
  // the type tree, we suspect they may well be). It is not guaranteed (and
  // not expected) that the contents of this vector are sorted. We can get
  // away with this only because we do not allow access to the contents of
  // this set until it is requested that we turn it into a flat_hash_set.
  std::vector<SelectorAccessPath> inner_vec_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_SELECTOR_ACCESS_PATH_SET_H_
