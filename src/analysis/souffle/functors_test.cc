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

#include "src/analysis/souffle/functors.h"

#include "souffle/datastructure/RecordTableImpl.h"
#include "souffle/datastructure/SymbolTableImpl.h"
#include "src/common/testing/gtest.h"

namespace raksha::analsis::souffle {

namespace {
class LogFunctorTest : public testing::TestWithParam<std::tuple<float, float>> {
 protected:
  ::souffle::SymbolTableImpl symbol_table_;
  ::souffle::SpecializedRecordTable<1> record_table_;
};

TEST_P(LogFunctorTest, LogFunctorTest) {
  const auto &[log_argument, log_result] = GetParam();
  ::souffle::RamDomain log_argument_ramdomain =
      ::souffle::ramBitCast<::souffle::RamDomain>(log_argument);
  ::souffle::RamDomain log_result_ramdomain =
      log_wrapper_cxx(&symbol_table_, &record_table_, log_argument_ramdomain);

  EXPECT_THAT(stof(symbol_table_.decode(log_result_ramdomain)), log_result);
}

// tuples of input and expected result for the LogFunctorTest case
constexpr std::tuple<float, float> kLogArgumentsAndReults[] = {
    {10, 1}, {45.6787987, 1.659715}, {2, 0.301030}, {5, 0.698970}, {100, 2}};

INSTANTIATE_TEST_SUITE_P(LogFunctorTest, LogFunctorTest,
                         testing::ValuesIn(kLogArgumentsAndReults));
}  // namespace
}  // namespace raksha::analsis::souffle
