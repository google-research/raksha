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
#include "src/xform_to_datalog/manifest_datalog_facts_to_datalog_test.h"

namespace raksha::xform_to_datalog {

TEST_P(ManifestDatalogFactsToDatalogTest,
       IncludesManifestFactsWithCorrectPrefixAndSuffix) {
  DatalogFacts datalog_facts(GetDatalogFacts());
  EXPECT_EQ(datalog_facts.ToDatalog(), expected_result_string_);
}

INSTANTIATE_TEST_SUITE_P(
    DatalogFactsTest, ManifestDatalogFactsToDatalogTest,
    testing::Values(
        std::tuple<
            std::vector<raksha::ir::InstanceFact<raksha::ir::TagClaim>>,
            std::vector<raksha::ir::InstanceFact<raksha::ir::TagCheck>>,
            std::vector<raksha::ir::InstanceFact<raksha::ir::Edge>>,
            std::string>{ {}, {}, {}, R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
.output checkHasTag

// Claims:


// Checks:


// Edges:

)" },
   std::tuple<
      std::vector<raksha::ir::InstanceFact<raksha::ir::TagClaim>>,
      std::vector<raksha::ir::InstanceFact<raksha::ir::TagCheck>>,
      std::vector<raksha::ir::InstanceFact<raksha::ir::Edge>>,
      std::string>{ {
      ir::CreateImmediateInstanceFact(ir::TagClaim(
                    "particle",
                      ir::AccessPath(
                          ir::AccessPathRoot(
                              ir::HandleConnectionAccessPathRoot(
                                  "recipe", "particle", "out")),
                      ir::AccessPathSelectors()),
                      true,
                      "tag")) }, {}, {}, R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
.output checkHasTag

// Claims:
claimHasTag("particle", "recipe.particle.out", "tag").

// Checks:


// Edges:

)"} ));

}  // namespace raksha::xform_to_datalog
