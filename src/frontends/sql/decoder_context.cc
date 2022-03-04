#include "src/frontends/sql/decoder_context.h"

namespace raksha::frontends::sql {

using ir::NamedValueMap;
using ir::Operation;
using ir::Value;

static NamedValueMap MakeNamedValueMapFromVector(absl::string_view prefix,
                                                 std::vector<Value> vec,
                                                 NamedValueMap map) {
  uint64_t i = 0;
  for (auto iter = std::make_move_iterator(vec.begin());
       iter != std::make_move_iterator(vec.end()); ++iter) {
    auto insert_result = map.insert({absl::StrCat(prefix, i), *iter});
    CHECK(insert_result.second)
        << "Collision on key " << insert_result.first->first;
    ++i;
  }
  return map;
}

const Operation &DecoderContext::MakeMergeOperation(
    std::vector<Value> direct_inputs, std::vector<Value> control_inputs) {
  // Add the direct and control inputs into the merge with predictable
  // prefixes.
  return top_level_block_builder_.AddOperation(
      merge_operator_,
      /*attributes=*/{},
      MakeNamedValueMapFromVector(
          "control_input_", std::move(control_inputs),
          MakeNamedValueMapFromVector("direct_input_", std::move(direct_inputs),
                                      {})));
}

}  // namespace raksha::frontends::sql
