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

#ifndef SRC_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_TO_DATALOG_TEST_H_
#define SRC_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_TO_DATALOG_TEST_H_

#include <string>
#include <vector>

#include "src/ir/edge.h"
#include "src/ir/instance_fact.h"
#include "src/ir/tag_claim.h"
#include "src/ir/tag_check.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"

// A test fixture used for constructing a ManifestDatalogFacts from the given
// arguments, each of which is already instantiated with an instantiator that
// does not have to be handled by this class. Test parameterization is
// unhappy with types that cannot be copied (ManifestDatalogFacts is one of
// those due to the owned instantiators), so it's useful to have a fixture
// that will construct a ManifestDatalogFacts from arguments.

namespace raksha::xform_to_datalog {

class ManifestDatalogFactsToDatalogTest :
   public testing::TestWithParam<
    std::tuple<
      std::vector<raksha::ir::InstanceFact<raksha::ir::TagClaim>>,
      std::vector<raksha::ir::InstanceFact<raksha::ir::TagCheck>>,
      std::vector<raksha::ir::InstanceFact<raksha::ir::Edge>>,
      std::string>> {
 protected:
  ManifestDatalogFactsToDatalogTest()
    : expected_result_string_(std::get<3>(GetParam())) {}

  // Note: We provide the ManifestDatalogFacts to the test via this method
  // rather than the usual way of via a protected field to work around the
  // single-copy property of std::unique_ptr.
  ManifestDatalogFacts GetDatalogFacts() const {
    return ManifestDatalogFacts(
        std::get<0>(GetParam()), std::get<1>(GetParam()),
        std::get<2>(GetParam()), {}, {});
  }

  std::string expected_result_string_;
};

}  // namespace raksha::xform_to_datalog

#endif  // SRC_XFORM_TO_DATALOG_MANIFEST_DATALOG_FACTS_TO_DATALOG_TEST_H_
