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
//-----------------------------------------------------------------------------

#include "src/common/utils/fold.h"

#include "absl/strings/str_join.h"
#include "src/common/testing/gtest.h"

namespace raksha::common::utils {

struct IntVecAndSum {
  std::vector<int> int_vec;
  int sum;
};

class SumFoldTest : public testing::TestWithParam<IntVecAndSum> {};

TEST_P(SumFoldTest, SumFoldTest) {
  const auto &[vec, expected_result] = GetParam();
  auto sum_fn = [](int sum_so_far, int num) { return sum_so_far + num; };
  EXPECT_EQ(fold(vec, 0, sum_fn), expected_result);
  EXPECT_EQ(rfold(vec, 0, sum_fn), expected_result);
}

static const IntVecAndSum kIntVecsAndSum[] = {
    {.int_vec = {}, .sum = 0},
    {.int_vec = {1}, .sum = 1},
    {.int_vec = {1, 2, 3, 4}, .sum = 10},
    {.int_vec = {-1, 5, 2, -8}, .sum = -2}};

INSTANTIATE_TEST_SUITE_P(SumFoldTest, SumFoldTest,
                         testing::ValuesIn(kIntVecsAndSum));

struct StringVecsAndConcatResults {
  std::vector<std::string> string_vec;
  std::string concat_result;
  std::string reverse_concat_result;
};

class ConcatFoldTest
    : public testing::TestWithParam<StringVecsAndConcatResults> {};

TEST_P(ConcatFoldTest, ConcatFoldTest) {
  const auto &[string_vec, concat_result, reverse_concat_result] = GetParam();
  auto concat_fn = [](std::string concat_so_far, const std::string &input) {
    // Yes, this is bad form in general, but the quadraticness shouldn't matter
    // for a test.
    return concat_so_far + input;
  };
  EXPECT_EQ(fold(string_vec, std::string(""), concat_fn), concat_result);
  EXPECT_EQ(rfold(string_vec, std::string(""), concat_fn),
            reverse_concat_result);
}

static const StringVecsAndConcatResults kSampleStringVecsAndConcatResults[] = {
    {.string_vec = {}, .concat_result = "", .reverse_concat_result = ""},
    {.string_vec = {"x"}, .concat_result = "x", .reverse_concat_result = "x"},
    {.string_vec = {"hello", "world"},
     .concat_result = "helloworld",
     .reverse_concat_result = "worldhello"},
    {.string_vec = {"a", "b", "c", "d", "e"},
     .concat_result = "abcde",
     .reverse_concat_result = "edcba"}};

INSTANTIATE_TEST_SUITE_P(ConcatFoldTest, ConcatFoldTest,
                         testing::ValuesIn(kSampleStringVecsAndConcatResults));

// Box and unbox some strings via `fold`. This mainly tests that we can handle
// an uncopyable accumulator (ie, the vector of unique pointers that we build
// up) and an uncopyable input vector.
class BoxFoldTest : public testing::TestWithParam<std::vector<std::string>> {};

TEST_P(BoxFoldTest, BoxFoldTest) {
  const std::vector<std::string> &string_vec = GetParam();
  std::vector<std::unique_ptr<std::string>> boxed_strings =
      fold(string_vec, std::vector<std::unique_ptr<std::string>>(),
           [](std::vector<std::unique_ptr<std::string>> boxed_so_far,
              const std::string &input) {
             boxed_so_far.push_back(std::make_unique<std::string>(input));
             return boxed_so_far;
           });
  std::vector<std::string> unboxed_strings =
      fold(boxed_strings, std::vector<std::string>(),
           [](std::vector<std::string> unboxed_so_far,
              const std::unique_ptr<std::string> &to_unbox) {
             unboxed_so_far.push_back(*to_unbox);
             return unboxed_so_far;
           });
  EXPECT_THAT(unboxed_strings, testing::ElementsAreArray(string_vec));
}

static const std::vector<std::string> kSampleStringVecs[] = {
    {}, {"x"}, {"hello", "world"}, {"a", "b", "c", "d", "e"}};

INSTANTIATE_TEST_SUITE_P(BoxFoldTest, BoxFoldTest,
                         testing::ValuesIn(kSampleStringVecs));

using RawPointerFoldTest = SumFoldTest;

// Here we show the iterator-taking `fold` doing something that the
// container-taking folds cannot: folding over a pair of "iterators" that are
// actually just pointers to a contiguous chunk of memory.
TEST_P(RawPointerFoldTest, RawPointerFoldTest) {
  const auto &[vec, expected_result] = GetParam();
  const int *begin_ptr = vec.data();
  const int *end_ptr = vec.data() + vec.size();
  auto sum_fn = [](int sum_so_far, int num) { return sum_so_far + num; };
  EXPECT_THAT(fold(begin_ptr, end_ptr, 0, sum_fn), expected_result);
}

INSTANTIATE_TEST_SUITE_P(RawPointerFoldTest, RawPointerFoldTest,
                         testing::ValuesIn(kIntVecsAndSum));

}  // namespace raksha::common::utils
