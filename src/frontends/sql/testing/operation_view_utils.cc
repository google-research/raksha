#include "operation_view_utils.h"

#include "absl/strings/match.h"
#include "absl/strings/numbers.h"

namespace raksha::frontends::sql::testing {

std::optional<uint64_t> ExtractIdxAfterPrefix(
    absl::string_view str, absl::string_view prefix) {
  if (!absl::StartsWith(str, prefix)) return std::nullopt;
  uint64_t result = 0;
  std::string index_str(str.substr(prefix.size()));
  if (!absl::SimpleAtoi(index_str, &result)) return std::nullopt;
  return result;
}

std::vector<ir::Value> GetVecWithPrefix(const ir::NamedValueMap &map,
                                               absl::string_view prefix) {
  uint64_t inferred_vec_length = 0;

  // The numbering of inputs should be dense. Thus, we can find the largest
  // index with the given prefix, add one to it, and consider that the
  // length of the vector.
  for (auto &[key, value] : map) {
    if (std::optional<uint64_t> opt_idx =
            ExtractIdxAfterPrefix(key, prefix)) {
      inferred_vec_length = std::max(inferred_vec_length, *opt_idx + 1);
    }
  }

  // For each item with the prefix up to the inferred length, get the value
  // and put it in the vector. If the numbering is not dense as we expect,
  // fail.
  std::vector<ir::Value> result;
  result.reserve(inferred_vec_length);
  for (uint64_t i = 0; i < inferred_vec_length; ++i) {
    auto find_result = map.find(absl::StrCat(prefix, i));
    CHECK(find_result != map.end())
        << "Found a hole in the key numbering: " << find_result->first;
    result.push_back(find_result->second);
  }
  return result;
}

}  // namespace raksha::frontends::sql::testing
