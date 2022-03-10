#ifndef SRC_FRONTENDS_SQL_TESTING_OPERATION_VIEW_UTILS_H_
#define SRC_FRONTENDS_SQL_TESTING_OPERATION_VIEW_UTILS_H_

#include <cstdint>
#include <optional>
#include <vector>

#include "absl/strings/string_view.h"
#include "src/ir/value.h"

namespace raksha::frontends::sql::testing {

std::optional<uint64_t> ExtractIdxAfterPrefix(
      absl::string_view str, absl::string_view prefix);

std::vector<ir::Value> GetVecWithPrefix(const ir::NamedValueMap &map,
                                               absl::string_view prefix);
}  // namespace raksha::frontends::sql::testing

#endif  // SRC_FRONTENDS_SQL_TESTING_OPERATION_VIEW_UTILS_H_
