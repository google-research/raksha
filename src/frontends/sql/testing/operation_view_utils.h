#ifndef SRC_FRONTENDS_SQL_TESTING_OPERATION_VIEW_UTILS_H_
#define SRC_FRONTENDS_SQL_TESTING_OPERATION_VIEW_UTILS_H_

#include <cstdint>
#include <optional>
#include <vector>

#include "absl/strings/string_view.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql::testing {

// This helper expects that the provided `str` is of the form `prefix_[0-9]+`.
// This function skips past the prefix, interprets the number as an unsigned
// int, and returns it. If any of those assumptions are not met, it returns
// std::nullopt instead.
std::optional<uint64_t> ExtractIdxAfterPrefix(
      absl::string_view str, absl::string_view prefix);

// This method assumes that, within the given `NamedValueMap`, there are a
// subset of elements that start with some prefix `prefix` and end with a
// numeric index. Further, it assumes that those indicies are densely
// clustered and starting at 0, such that they can be described as an array.
// This method produces a `vector` from the values with that prefix, with the
// trailing numeric suffixes being translated into positions in the resulting
// vector. If the density assumption is not met, this method asserts.
std::vector<ir::Value> GetVecWithPrefix(const ir::NamedValueMap &map,
                                               absl::string_view prefix);
}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_OPERATION_VIEW_UTILS_H_
