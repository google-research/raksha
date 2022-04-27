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

#include "src/common/utils/refct_ptr.h"

#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"

namespace raksha::common::utils {

TEST(NullRefctPtr, NullRefctPtr) {
  refct_ptr<int> null_refct_ptr;
  auto inner_intrusive_ptr = null_refct_ptr.get_intrusive_ptr();
  EXPECT_TRUE(inner_intrusive_ptr == nullptr);
  EXPECT_FALSE(null_refct_ptr);
}

class TestIntRefctPtr : public testing::TestWithParam<int> {};

TEST_P(TestIntRefctPtr, TestIntRefctPtr) {
  int param = GetParam();
  refct_ptr<int> int_ptr = make_refct_ptr<int>(param);
  EXPECT_EQ(*int_ptr, param);
  EXPECT_TRUE(int_ptr);
  const refct_ptr<int> const_int_ptr = int_ptr;
  EXPECT_EQ(*const_int_ptr, param);
  EXPECT_EQ(int_ptr.get_intrusive_ptr(), const_int_ptr.get_intrusive_ptr());
  EXPECT_TRUE(const_int_ptr);
}

static int kSampleInts[] = {0, -1, 100, -5};

INSTANTIATE_TEST_SUITE_P(TestIntRefctPtr, TestIntRefctPtr,
                         testing::ValuesIn(kSampleInts));

class IntStringPair {
 public:
  IntStringPair(int i, absl::string_view s) : int_(i), string_(s) {}

  int get_int() const { return int_; }
  absl::string_view get_string() const { return string_; }

 private:
  int int_;
  std::string string_;
};

class TestIntStringPairPtr
    : public testing::TestWithParam<std::tuple<int, absl::string_view>> {};

TEST_P(TestIntStringPairPtr, TestIntStringPairPtr) {
  auto [i, s] = GetParam();
  refct_ptr<IntStringPair> ptr = make_refct_ptr<IntStringPair>(i, s);
  EXPECT_EQ(ptr->get_int(), i);
  EXPECT_EQ(ptr->get_string(), s);
  EXPECT_TRUE(ptr);
  const refct_ptr<IntStringPair> const_ptr = ptr;
  EXPECT_EQ(const_ptr->get_int(), i);
  EXPECT_EQ(const_ptr->get_string(), s);
  EXPECT_TRUE(const_ptr);
}

static absl::string_view kSampleStrings[] = {"", "abc", "def", "foo", "!!!"};

INSTANTIATE_TEST_SUITE_P(TestIntStringPairPtr, TestIntStringPairPtr,
                         testing::Combine(testing::ValuesIn(kSampleInts),
                                          testing::ValuesIn(kSampleStrings)));

}  // namespace raksha::common::utils
