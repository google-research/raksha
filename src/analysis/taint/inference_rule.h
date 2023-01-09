//-----------------------------------------------------------------------------
// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//-----------------------------------------------------------------------------
#ifndef SRC_ANALYSIS_TAINT_INFERENCE_RULE_H_
#define SRC_ANALYSIS_TAINT_INFERENCE_RULE_H_

#include <cstdint>
#include <variant>
#include <vector>

namespace raksha::analysis::taint {

// A type for representing tag ids in the inference rules.
using TagId = uint64_t;

// Captures the fact that the given input should have a specific integrity.
struct InputHasIntegrity {
  uint64_t input_index;
  TagId tag;
};

inline bool operator==(const InputHasIntegrity& lhs,
                       const InputHasIntegrity& rhs) {
  return lhs.input_index == rhs.input_index && lhs.tag == rhs.tag;
}

inline bool operator!=(const InputHasIntegrity& lhs,
                       const InputHasIntegrity& rhs) {
  return !(lhs == rhs);
}

// Captures how the tag of a specific output index should be changed.
struct CopyInput {
  uint64_t input_index;
};

inline bool operator==(const CopyInput& lhs, const CopyInput& rhs) {
  return lhs.input_index == rhs.input_index;
}

inline bool operator!=(const CopyInput& lhs, const CopyInput& rhs) {
  return !(lhs == rhs);
}

// Merge (union or intersect) the integrity tags of the given input indices.
struct MergeIntegrityTags {
  enum class Kind { kUnion, kIntersect };
  Kind kind;
  std::vector<uint64_t> input_indices;
};

inline bool operator==(const MergeIntegrityTags& lhs,
                       const MergeIntegrityTags& rhs) {
  return lhs.kind == rhs.kind && lhs.input_indices == rhs.input_indices;
}

inline bool operator!=(const MergeIntegrityTags& lhs,
                       const MergeIntegrityTags& rhs) {
  return !(lhs == rhs);
}

// Modify an integrity or secrecy tag.
struct ModifyTag {
  enum class Kind { kAddSecrecy, kRemoveSecrecy, kAddIntegrity };
  Kind kind;
  TagId tag;
};

inline bool operator==(const ModifyTag& lhs, const ModifyTag& rhs) {
  return lhs.kind == rhs.kind && lhs.tag == rhs.tag;
}

inline bool operator!=(const ModifyTag& lhs, const ModifyTag& rhs) {
  return !(lhs == rhs);
}

// The conclusion of an taint-related inference rule.
using Conclusion = std::variant<CopyInput, ModifyTag, MergeIntegrityTags>;

// Describes the natural semantics of taint analysis for an IR operation.
struct InferenceRule {
  Conclusion conclusion;
  std::vector<InputHasIntegrity> premises;
};

inline bool operator==(const InferenceRule& lhs, const InferenceRule& rhs) {
  return lhs.conclusion == rhs.conclusion && lhs.premises == rhs.premises;
}

inline bool operator!=(const InferenceRule& lhs, const InferenceRule& rhs) {
  return !(lhs == rhs);
}

}  // namespace raksha::analysis::taint

#endif  // SRC_ANALYSIS_TAINT_INFERENCE_RULE_H_
