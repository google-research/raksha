//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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
//----------------------------------------------------------------------------

#include "src/ir/predicate_textproto_to_rule_body_testdata.h"

#include "src/test_utils/dl_string_extractor/datalog_string.h"

// This is not in a namespace matching the directory tree nor in the related
// header for a reason. We are using this to perform linking magic to fill in
// GatherDatalogStrings in the dl_string_test_file_generator source file.
namespace raksha::test_utils::dl_string_extractor {

std::vector<DatalogString> GatherDatalogStrings() {
  std::vector<DatalogString> results;
  for (const auto &[textproto, dl_string] :
      raksha::ir::predicate_textproto_to_rule_body_format) {
    results.push_back(DatalogString(
        kDlRuleBody,
        absl::Substitute(dl_string, "mayHaveTag(\"example_ap\", _")));
  }
  return results;
}

}
