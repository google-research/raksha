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
#include <string>

#include "absl/strings/string_view.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/ops/merge_op.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/utils.h"

namespace raksha::frontends::sql {

namespace {

using ir::Block;
using ir::IRContext;
using ir::Operation;
using ir::Value;
using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

constexpr std::optional<absl::string_view> kSampleExprNames[] = {
    {}, {"name1"}, {"another_name"}};

// We test the decoding of the child `Operation`s in other tests. We also
// ensure that we can correctly make a `MergeOperation` from two vectors of
// inputs in another test. Therefore, this is strictly about whether we can
// decode the `MergeOperation` proto. This test accepts 3 parameters: the name,
// the number of direct inputs, and the number of control inputs. We will
// generate the given number of `Literal` values on the input proto.
class DecodeMergeOpTest
    : public TestWithParam<
          std::tuple<std::optional<absl::string_view>, uint64_t, uint64_t>> {
 protected:
  DecodeMergeOpTest() : ir_context_(), decoder_context_(ir_context_) {}

  std::string GenerateIdListProto(uint64_t len, uint64_t id_start) const {
    std::vector<std::string> literal_vec;
    for (uint64_t id = id_start; id < id_start + len; ++id) {
      literal_vec.push_back(absl::StrFormat("%u", id));
    }
    return absl::StrJoin(literal_vec, ",");
  }

  std::string GetTextproto() const {
    auto &[name, direct_size, control_size] = GetParam();
    // Create the `MergeExpression` proto.
    std::string name_str =
        (name.has_value()) ? absl::StrFormat(R"(name: "%s")", *name) : "";
    std::string direct_input_list_contents =
        GenerateIdListProto(direct_size, 1);
    std::string control_input_list_contents =
        GenerateIdListProto(control_size, 1 + direct_size);
    std::vector<uint64_t> ids_to_be_filled;
    for (uint64_t i = 1; i <= direct_size + control_size; ++i) {
      ids_to_be_filled.push_back(i);
    }
    return testing::CreateExprArenaTextprotoWithLiteralsPrefix(
        absl::StrFormat("{ id: 100 expression: { %s merge_operation: { inputs: "
                        "[ %s ] control_inputs: [ %s ] } } }",
                        name_str, direct_input_list_contents,
                        control_input_list_contents),
        ids_to_be_filled);
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

TEST_P(DecodeMergeOpTest, DecodeMergeOpTest) {
  ExpressionArena exprArena;
  EXPECT_TRUE(
      google::protobuf::TextFormat::ParseFromString(GetTextproto(), &exprArena))
      << "Could not decode expr";
  Value value = DecodeExpressionArena(exprArena, decoder_context_);
  const Block &top_level_block = decoder_context_.BuildTopLevelBlock();

  // Set up is finished, now check our expectations.

  // We always have the value of the `MergeOp` that we are decoding be 1 to
  // prevent collisions with child values.
  const Operation &op = testing::UnwrapDefaultOperationResult(
      testing::UnwrapTopLevelSqlOutputOp(value));
  EXPECT_EQ(op.parent(), &top_level_block);

  auto merge_op = SqlOp::GetIf<MergeOp>(op);
  ASSERT_NE(merge_op, nullptr);
  auto &[optional_name, direct_size, control_size] = GetParam();
  EXPECT_THAT(merge_op->GetDirectInputs(),
              ::testing::BeginEndDistanceIs(direct_size));
  EXPECT_THAT(merge_op->GetControlInputs(),
              ::testing::BeginEndDistanceIs(control_size));
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
