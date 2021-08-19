#include "selector_access_path_set.h"
#include "selector.h"

namespace raksha::ir {

void SelectorAccessPathSet::AddAll(SelectorAccessPathSet other) {
  for (SelectorAccessPath &path : other.inner_vec_) {
    inner_vec_.push_back(std::move(path));
  }
}

SelectorAccessPathSet SelectorAccessPathSet::Union(
    SelectorAccessPathSet set1, SelectorAccessPathSet set2) {
  SelectorAccessPathSet result;

  result.AddAll(std::move(set1));
  result.AddAll(std::move(set2));

  return result;
}

SelectorAccessPathSet SelectorAccessPathSet::Intersect(
    SelectorAccessPathSet set1, SelectorAccessPathSet set2) {
  SelectorAccessPathSet result;

  absl::flat_hash_set<SelectorAccessPath> hash_set1;
  for (SelectorAccessPath &path : set1.inner_vec_) {
    hash_set1.insert(std::move(path));
  }

  for (SelectorAccessPath &path : set2.inner_vec_) {
    if (hash_set1.contains(path)) {
      result.Add(std::move(path));
    }
  }

  return result;
}

SelectorAccessPathSet SelectorAccessPathSet::AddParentToAll(
    Selector parent_selector,
    SelectorAccessPathSet child_set) {
  SelectorAccessPathSet result;

  for (SelectorAccessPath &path : child_set.inner_vec_) {
    result.inner_vec_.push_back(
        SelectorAccessPath(parent_selector, std::move(path)));
  }

  return result;
}

absl::flat_hash_set<SelectorAccessPath>
    SelectorAccessPathSet::MoveIntoAbslSet() {
  absl::flat_hash_set<SelectorAccessPath> result;

  for (SelectorAccessPath &selector_access_path : inner_vec_) {
    result.insert(std::move(selector_access_path));
  }
  return result;
}

}  // namespace raksha::ir
