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

#include "google/protobuf/text_format.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/utils.h"

namespace raksha::frontends::sql {

namespace {

using ir::IRContext;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

struct TextprotoDeathMessagePair {
  std::string textproto;
  std::string death_message;
};

class DecodeExprDeathTest : public TestWithParam<TextprotoDeathMessagePair> {
 protected:
  DecodeExprDeathTest() : ir_context_(), decoder_context_(ir_context_) {}

  ExpressionArena GetExprArena() {
    ExpressionArena exprArena;
    EXPECT_TRUE(google3_proto_compat::TextFormat::ParseFromString(
        testing::CreateExprArenaTextprotoWithLiteralsPrefix(
            GetParam().textproto, {}),
        &exprArena))
        << "Could not decode exprArena";
    return exprArena;
  }

  IRContext ir_context_;
  DecoderContext decoder_context_;
};

TEST_P(DecodeExprDeathTest, DecodeExprDeathTest) {
  ExpressionArena exprArena = GetExprArena();
  EXPECT_DEATH({ DecodeExpressionArena(exprArena, decoder_context_); },
               GetParam().death_message);
}

const TextprotoDeathMessagePair kTextprotoDeathMessagePairs[] = {
    {.textproto = R"()",
     .death_message = "Expected at least one expression to be provided in the "
                      "ExpressionArena."},
    {.textproto = R"({})",
     .death_message = "Required field expr_variant not set."},
    {.textproto = R"({id: 1 expression: { merge_operation: { } } })",
     .death_message = "Each MergeOperation is expected to have at least one "
                      "non-control input."},
    {.textproto =
         R"({ id: 1 expression: { merge_operation: { inputs: [ 1000 ]} } })",
     .death_message = "Could not find a value with id 1000."},
    {.textproto =
         R"({ id: 1 expression: { merge_operation: { inputs: [ 0 ]} } })",
     .death_message =
         "Attempt to get a value with id 0, which is not a legal value id."},
};

INSTANTIATE_TEST_SUITE_P(DecodeExprDeathTest, DecodeExprDeathTest,
                         ValuesIn(kTextprotoDeathMessagePairs));

}  // anonymous namespace

}  // namespace raksha::frontends::sql
