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

#ifndef SRC_COMMON_UTILS_TEST_DL_STRING_EXTRACTOR_DATALOG_STRING_H_
#define SRC_COMMON_UTILS_TEST_DL_STRING_EXTRACTOR_DATALOG_STRING_H_

#include <string>
#include <string_view>

namespace raksha::utils::test::dl_string_extractor {

enum DatalogStringKind {
  kDlRuleBody,
};

// A string that is a snippet of datalog, marked with the grammatical part of
// Datalog it represents.
class DatalogString {
 public:
  DatalogString(DatalogStringKind kind, std::string dl_string)
      : kind_(kind), dl_string_(std::move(dl_string)) {}

  DatalogStringKind kind() const { return kind_; }

  std::string_view dl_string() const { return dl_string_; }

 private:
  DatalogStringKind kind_;
  std::string dl_string_;
};

}  // namespace raksha::utils::test::dl_string_extractor

#endif  // SRC_COMMON_UTILS_TEST_DL_STRING_EXTRACTOR_DATALOG_STRING_H_
