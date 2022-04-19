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

#include "src/backends/policy_enforcement/souffle/fact.h"

#include "absl/strings/string_view.h"
#include "src/backends/policy_enforcement/souffle/value.h"
#include "src/common/testing/gtest.h"

namespace raksha::backends::policy_enforcement::souffle {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

static const char kIsAccessPathName[] = "isAccessPath";

using IsAccessPathFact = Fact<kIsAccessPathName, Symbol>;

class IsAccessPathFactTest : public TestWithParam<absl::string_view> {};

TEST_P(IsAccessPathFactTest, IsAccessPathFactTest) {
  absl::string_view symbol_string = GetParam();
  EXPECT_EQ(IsAccessPathFact(Symbol(symbol_string)).ToDatalogString(),
            absl::StrFormat(R"(isAccessPath("%s").)", symbol_string));
}

static const absl::string_view kSampleAccessPathStrings[] = {"", "P1.foo",
                                                             "P2.handle1.x"};

INSTANTIATE_TEST_SUITE_P(IsAccessPathFactTest, IsAccessPathFactTest,
                         ValuesIn(kSampleAccessPathStrings));

static const char kSimpleAdtName[] = "SimpleAdt";
static const char kUnitName[] = "Unit";
static const char kForkName[] = "Fork";

using SimpleAdt = Adt<kSimpleAdtName>;

using UnitAdtBranch = AdtBranch<kSimpleAdtName, kUnitName>;

static const char kOneOfEachName[] = "oneOfEach";

using OneOfEachFact =
    Fact<kOneOfEachName, Number, Symbol, Record<Number, Symbol>, SimpleAdt>;

TEST(OneOfEachFactTest, OneOfEachFactTest) {
  EXPECT_EQ(OneOfEachFact(Number(5), Symbol("foo"),
                          Record<Number, Symbol>(Number(3), Symbol("bar")),
                          SimpleAdt(UnitAdtBranch()))
                .ToDatalogString(),
            R"(oneOfEach(5, "foo", [3, "bar"], $Unit{}).)");
}

}  // namespace raksha::backends::policy_enforcement::souffle
