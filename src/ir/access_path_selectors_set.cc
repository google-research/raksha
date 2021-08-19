#include "access_path_selectors_set.h"
#include "selector.h"

namespace raksha::ir {

void AccessPathSelectorsSet::AddAll(AccessPathSelectorsSet other) {
  for (AccessPathSelectors &path : other.inner_vec_) {
    inner_vec_.push_back(std::move(path));
  }
}

AccessPathSelectorsSet AccessPathSelectorsSet::Union(
    AccessPathSelectorsSet set1, AccessPathSelectorsSet set2) {
  AccessPathSelectorsSet result;

  result.AddAll(std::move(set1));
  result.AddAll(std::move(set2));

  return result;
}

AccessPathSelectorsSet AccessPathSelectorsSet::Intersect(
    AccessPathSelectorsSet set1, AccessPathSelectorsSet set2) {
  AccessPathSelectorsSet result;

  absl::flat_hash_set<AccessPathSelectors> hash_set1;
  for (AccessPathSelectors &path : set1.inner_vec_) {
    hash_set1.insert(std::move(path));
  }

  for (AccessPathSelectors &path : set2.inner_vec_) {
    if (hash_set1.contains(path)) {
      result.Add(std::move(path));
    }
  }

  return result;
}

AccessPathSelectorsSet AccessPathSelectorsSet::AddParentToAll(
    Selector parent_selector,
    AccessPathSelectorsSet child_set) {
  AccessPathSelectorsSet result;

  for (AccessPathSelectors &path : child_set.inner_vec_) {
    result.inner_vec_.push_back(
        AccessPathSelectors(parent_selector, std::move(path)));
  }

  return result;
}

absl::flat_hash_set<AccessPathSelectors>
    AccessPathSelectorsSet::MoveIntoAbslSet() {
  absl::flat_hash_set<AccessPathSelectors> result;

  for (AccessPathSelectors &selector_access_path : inner_vec_) {
    result.insert(std::move(selector_access_path));
  }
  return result;
}

}  // namespace raksha::ir
