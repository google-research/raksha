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

#include "absl/strings/string_view.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/merge_operation_view.h"
#include "src/frontends/sql/testing/utils.h"

namespace raksha::frontends::sql {

namespace {

using ir::Attribute;
using ir::Block;
using ir::IRContext;
using ir::Operation;
using ir::Storage;
using ir::Value;
using ir::types::TypeBase;
using ir::value::OperationResult;
using ir::value::StoredValue;
using ::testing::Combine;
using ::testing::Eq;
using ::testing::IsEmpty;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::ResultOf;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;
using ::testing::Values;
using ::testing::ValuesIn;

constexpr std::optional<absl::string_view> kSampleExprNames[] = {
    {}, {"name1"}, {"another_name"}};

// We test the decoding of the child `Operation`s in other tests. We also
// ensure that we can correctly make a `MergeOperation` from two vectors of
// inputs in another test. Therefore, this is strictly about whether we can
// decode the `MergeOperation` proto. This test accepts 4 parameters: the id,
// the name, the number of direct inputs, and the number of control inputs.
// We will generate the given number of `Literal` values on the input proto.
class DecodeMergeOpTest
    : public TestWithParam<
          std::tuple<std::optional<absl::string_view>, uint64_t, uint64_t>> {
 protected:
  DecodeMergeOpTest() : ir_context_(), decoder_context_(ir_context_) {}

  std::string GenerateLiteralListContentsProto(uint64_t len,
                                               uint64_t id_start) const {
    std::vector<std::string> literal_vec;
    for (uint64_t id = id_start; id < id_start + len; ++id) {
      literal_vec.push_back(
          absl::StrFormat(R"({ id: %u literal: { literal_str: "lit" } })", id));
    }
    return absl::StrJoin(literal_vec, ",");
  }

  std::string GetTextproto() const {
    auto &[name, direct_size, control_size] = GetParam();
    std::string name_str =
        (name.has_value()) ? absl::StrFormat(R"(name: "%s")", *name) : "";
    // Start from 2 so that we can assign 1 to the top level MergeOperation.
    std::string direct_input_list_contents =
        GenerateLiteralListContentsProto(direct_size, 2);
    std::string control_input_list_contents =
        GenerateLiteralListContentsProto(control_size, 2 + direct_size);
    return absl::StrFormat(
        "id: 1 %s merge_operation: { inputs: [ %s ] control_inputs: [ %s ] }",
        name_str, direct_input_list_contents, control_input_list_contents);
  }

  Value GetDecodedValue() {
    Expression expr;
    EXPECT_TRUE(
        google::protobuf::TextFormat::ParseFromString(GetTextproto(), &expr))
        << "Could not decode expr";
    return DecodeExpression(expr, decoder_context_);
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

TEST_P(DecodeMergeOpTest, DecodeMergeOpTest) {
  Expression expr;
  EXPECT_TRUE(
      google::protobuf::TextFormat::ParseFromString(GetTextproto(), &expr))
      << "Could not decode expr";
  Value value = DecodeExpression(expr, decoder_context_);
  const Block &top_level_block = decoder_context_.BuildTopLevelBlock();

  // Set up is finished, now check our expectations.

  // We always have the value of the `MergeOp` that we are decoding be 1 to
  // prevent collisions with child values.
  EXPECT_EQ(value, decoder_context_.GetValue(1));
  const Operation &op = testing::UnwrapDefaultOperationResult(value);
  EXPECT_EQ(op.parent(), &top_level_block);

  testing::MergeOperationView merge_operation_view(op);

  auto &[optional_name, direct_size, control_size] = GetParam();
  EXPECT_THAT(merge_operation_view.GetDirectInputs().size(), direct_size);
  EXPECT_THAT(merge_operation_view.GetControlInputs().size(), control_size);
}

static constexpr uint64_t kSampleControlInputLengths[] = {0, 1, 5, 10};
// 0 is not allowed for direct input lengths. Array is otherwise the same as
// the control input lengths.
static constexpr uint64_t kSampleDirectInputLengths[] = {1, 5, 10};

INSTANTIATE_TEST_SUITE_P(DecodeMergeOpTest, DecodeMergeOpTest,
                         Combine(ValuesIn(kSampleExprNames),
                                 ValuesIn(kSampleDirectInputLengths),
                                 ValuesIn(kSampleControlInputLengths)));

}  // anonymous namespace

}  // namespace raksha::frontends::sql
