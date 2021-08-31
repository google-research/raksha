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
//-----------------------------------------------------------------------------

#include "src/xform_to_datalog/datalog_facts.h"

#include "src/common/testing/gtest.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"

namespace raksha::xform_to_datalog {

namespace ir = raksha::ir;

constexpr char kDatalogFileTemplate[] =
    R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
%s
.output checkHasTag
)";

class DatalogFactsTest : public testing::TestWithParam<ManifestDatalogFacts> {};

TEST_P(DatalogFactsTest, IncludesManifestFactsWithCorrectPrefixAndSuffix) {
  const ManifestDatalogFacts &manifest_datalog_facts = GetParam();
  std::string expected_result_string =
      absl::StrFormat(kDatalogFileTemplate, manifest_datalog_facts.ToString());
  DatalogFacts datalog_facts(manifest_datalog_facts);
  EXPECT_EQ(datalog_facts.ToString(), expected_result_string);
}

INSTANTIATE_TEST_SUITE_P(
    DatalogFactsTest, DatalogFactsTest,
    testing::ValuesIn(
        {ManifestDatalogFacts({}, {}, {}),
         ManifestDatalogFacts(
             {ir::TagClaim(ir::TagAnnotationOnAccessPath(
                 ir::AccessPath(
                     ir::AccessPathRoot(ir::HandleConnectionAccessPathRoot(
                         "recipe", "particle", "out")),
                     ir::AccessPathSelectors()),
                 "tag"))},
             {}, {})}));

}  // namespace raksha::xform_to_datalog
