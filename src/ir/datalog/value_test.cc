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

#include "src/ir/datalog/value.h"

#include <limits>

#include "absl/strings/numbers.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir::datalog {

using testing::Combine;
using testing::TestWithParam;
using testing::ValuesIn;

class NumberTest : public TestWithParam<int64_t> {};

TEST_P(NumberTest, NumberTest) {
  int64_t num = GetParam();
  Number number_value = Number(num);
  std::string datalog_str = number_value.ToDatalogString();
  int64_t parsed_int = 0;
  bool conversion_succeeds = absl::SimpleAtoi(datalog_str, &parsed_int);
  ASSERT_TRUE(conversion_succeeds);
  ASSERT_EQ(parsed_int, num);
}

static int64_t kSampleIntegerValues[] = {0, -1, 1,
                                         std::numeric_limits<long>::max(),
                                         std::numeric_limits<long>::min()};

INSTANTIATE_TEST_SUITE_P(NumberTest, NumberTest,
                         ValuesIn(kSampleIntegerValues));

class SymbolTest : public TestWithParam<absl::string_view> {};

TEST_P(SymbolTest, SymbolTest) {
  absl::string_view symbol = GetParam();
  Symbol symbol_value = Symbol(symbol);
  std::string symbol_str = symbol_value.ToDatalogString();
  ASSERT_EQ(symbol_str, "\"" + std::string(symbol) + "\"");
}

static absl::string_view kSampleSymbols[] = {"", "x", "foo", "hello_world"};

INSTANTIATE_TEST_SUITE_P(SymbolTest, SymbolTest, ValuesIn(kSampleSymbols));

using SimpleRecord = Record<Symbol, Number>;

TEST(SimpleRecordNilTest, SimpleRecordNilTest) {
  ASSERT_EQ(Record<SimpleRecord>().ToDatalogString(), "nil");
}

class SimpleRecordTest
    : public TestWithParam<std::tuple<absl::string_view, int64_t>> {};

TEST_P(SimpleRecordTest, SimpleRecordTest) {
  const auto [symbol, number] = GetParam();
  SimpleRecord record_value = SimpleRecord(Symbol(symbol), Number(number));
  ASSERT_EQ(record_value.ToDatalogString(),
            absl::StrFormat(R"(["%s", %d])", symbol, number));
}

INSTANTIATE_TEST_SUITE_P(SimpleRecordTest, SimpleRecordTest,
                         Combine(ValuesIn(kSampleSymbols),
                                 ValuesIn(kSampleIntegerValues)));

class NumList : public Record<Number, NumList> {
 public:
  using Record::Record;
};

struct NumListAndExpectedDatalog {
  const NumList *num_list_ptr;
  absl::string_view expected_datalog;
};

class NumListTest : public TestWithParam<NumListAndExpectedDatalog> {};

TEST_P(NumListTest, NumListTest) {
  const auto [num_list_ptr, expected_datalog] = GetParam();
  EXPECT_EQ(num_list_ptr->ToDatalogString(), expected_datalog);
}

static const NumList kEmptyNumList;
static const NumList kOneElementNumList = NumList(Number(5), NumList());
static const NumList kTwoElementNumList(Number(-30),
                                        NumList(Number(28), NumList()));

static NumListAndExpectedDatalog kListAndExpectedDatalog[] = {
    {.num_list_ptr = &kEmptyNumList, .expected_datalog = "nil"},
    {.num_list_ptr = &kOneElementNumList, .expected_datalog = "[5, nil]"},
    {.num_list_ptr = &kTwoElementNumList,
     .expected_datalog = "[-30, [28, nil]]"}};

INSTANTIATE_TEST_SUITE_P(NumListTest, NumListTest,
                         ValuesIn(kListAndExpectedDatalog));

using NumberSymbolPair = Record<Number, Symbol>;
using NumberSymbolPairPair = Record<NumberSymbolPair, NumberSymbolPair>;

class NumberSymbolPairPairTest
    : public TestWithParam<std::tuple<std::tuple<int64_t, absl::string_view>,
                                      std::tuple<int64_t, absl::string_view>>> {
};

TEST_P(NumberSymbolPairPairTest, NumberSymbolPairPairTest) {
  auto const &[pair1, pair2] = GetParam();
  auto const [number1, symbol1] = pair1;
  auto const [number2, symbol2] = pair2;
  NumberSymbolPair number_symbol_pair1 =
      NumberSymbolPair(Number(number1), Symbol(symbol1));
  NumberSymbolPair number_symbol_pair2 =
      NumberSymbolPair(Number(number2), Symbol(symbol2));
  NumberSymbolPairPair pair_pair(std::move(number_symbol_pair1),
                                 std::move(number_symbol_pair2));
  EXPECT_EQ(pair_pair.ToDatalogString(),
            absl::StrFormat(R"([[%d, "%s"], [%d, "%s"]])", number1, symbol1,
                            number2, symbol2));
}

INSTANTIATE_TEST_SUITE_P(
    NumberSymbolPairPairTest, NumberSymbolPairPairTest,
    Combine(Combine(ValuesIn(kSampleIntegerValues), ValuesIn(kSampleSymbols)),
            Combine(ValuesIn(kSampleIntegerValues), ValuesIn(kSampleSymbols))));

static constexpr char kNullBranchName[] = "Null";
static constexpr char kNumberBranchName[] = "Number";
static constexpr char kAddBranchName[] = "Add";

class ArithmeticAdt : public Adt {
  using Adt::Adt;
};

class NullBranch : public ArithmeticAdt {
 public:
  NullBranch() : ArithmeticAdt(kNullBranchName) {}
};

class NumberBranch : public ArithmeticAdt {
 public:
  NumberBranch(Number number) : ArithmeticAdt(kNumberBranchName) {
    arguments_.push_back(std::make_unique<Number>(number));
  }
};

class AddBranch : public ArithmeticAdt {
 public:
  AddBranch(ArithmeticAdt lhs, ArithmeticAdt rhs)
      : ArithmeticAdt(kAddBranchName) {
    arguments_.push_back(std::make_unique<ArithmeticAdt>(std::move(lhs)));
    arguments_.push_back(std::make_unique<ArithmeticAdt>(std::move(rhs)));
  }
};

struct AdtAndExpectedDatalog {
  const ArithmeticAdt *adt;
  absl::string_view expected_datalog;
};

class AdtTest : public TestWithParam<AdtAndExpectedDatalog> {};

TEST_P(AdtTest, AdtTest) {
  auto &[adt, expected_datalog] = GetParam();
  EXPECT_EQ(adt->ToDatalogString(), expected_datalog);
}

static const ArithmeticAdt kNull = ArithmeticAdt(NullBranch());
static const ArithmeticAdt kFive = ArithmeticAdt(NumberBranch(Number(5)));
static const ArithmeticAdt kTwo = ArithmeticAdt(NumberBranch(Number(2)));
static const ArithmeticAdt kFivePlusTwo =
    ArithmeticAdt(AddBranch(ArithmeticAdt(NumberBranch(Number(5))),
                            ArithmeticAdt(NumberBranch(Number(2)))));
static const ArithmeticAdt kFivePlusTwoPlusNull = ArithmeticAdt(
    AddBranch(ArithmeticAdt(NumberBranch(Number(5))),
              ArithmeticAdt(AddBranch(ArithmeticAdt(NumberBranch(Number(2))),
                                      ArithmeticAdt(NullBranch())))));

static const AdtAndExpectedDatalog kAdtAndExpectedDatalog[] = {
    {.adt = &kNull, .expected_datalog = "$Null()"},
    {.adt = &kFive, .expected_datalog = "$Number(5)"},
    {.adt = &kFivePlusTwo, .expected_datalog = "$Add($Number(5), $Number(2))"},
    {.adt = &kFivePlusTwoPlusNull,
     .expected_datalog = "$Add($Number(5), $Add($Number(2), $Null()))"}};

INSTANTIATE_TEST_SUITE_P(AdtTest, AdtTest, ValuesIn(kAdtAndExpectedDatalog));

}  // namespace raksha::ir::datalog
