//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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

#include "datalog_lowering_visitor.h"

#include "absl/strings/string_view.h"
#include "src/backends/policy_enforcement/souffle/value.h"
#include "src/common/testing/gtest.h"

namespace raksha::backends::policy_enforcement::souffle {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

struct IrOperationAndExpectedDatalogOperation {
  ir::Operation ir_operation;
  Operation operation;
};

class DatalogLoweringVisitOperationTest : public
    TestWithParam<ModuleAndExpectedRakshaDatalogFacts> {};

TEST_P(DatalogLoweringVisitOperationTest, DatalogLoweringVisitOperationTest) {
  const auto &[module, facts] = GetParam();
  ir::SsaNames ssa_names;
  DatalogLoweringVisitor visitor(ssa_names);
  module.Accept(visitor);
  EXPECT_EQ(visitor.datalog_facts().ToDatalogString(), facts.ToDatalogString());
}

static const ModuleAndExpectedRakshaDatalogFacts
kModuleAndExpectedRakshaDatalogFacts[] = {
    { .module = }
};

}  // namespace raksha::backends::policy_enforcement::souffle
