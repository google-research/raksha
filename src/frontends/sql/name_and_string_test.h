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

#ifndef SRC_FRONTENDS_SQL_ID_NAME_AND_STRING_TEST_H_
#define SRC_FRONTENDS_SQL_ID_NAME_AND_STRING_TEST_H_

#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/text_format.h"
#include "src/common/testing/gtest.h"
#include "src/frontends/sql/decode.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/testing/utils.h"
#include "src/ir/ir_context.h"

namespace raksha::frontends::sql {

// A value-parameterized test fixture used for both SourceTableColumns and
// Literals. Both of these constructs are basically just expressions
// containing a string value. However, the textproto that describes them is
// slightly different, and the IR that is generated is different. This
// superclass allows us to reuse the common logic while the virtual
// `GetTexprotoFormat` allows us to construct the textproto as necessary for
// each structure.
class NameAndStringTest
    : public ::testing::TestWithParam<
          std::tuple<std::optional<absl::string_view>, absl::string_view>> {
 protected:
  NameAndStringTest() : ir_context_(), decoder_context_(ir_context_) {}

  virtual absl::ParsedFormat<'s', 's'> GetTextprotoFormat() const = 0;

  std::string GetTextproto() const {
    auto &[name, str] = GetParam();
    std::string name_str =
        (name.has_value()) ? absl::StrFormat(R"(name: "%s")", *name) : "";
    return absl::StrFormat(
        "{ id: 1 expression: %s } ",
        absl::StrFormat(GetTextprotoFormat(), name_str, str));
  }

  ir::Value GetDecodedValue() {
    ExpressionArena exprArena;
    EXPECT_TRUE(google::protobuf::TextFormat::ParseFromString(
        testing::CreateExprArenaTextprotoWithLiteralsPrefix(GetTextproto(), {}),
        &exprArena))
        << "Could not decode expr";
    return testing::UnwrapTopLevelSqlOutputOp(
        DecodeExpressionArena(exprArena, decoder_context_));
  }

  ir::IRContext ir_context_;
  DecoderContext decoder_context_;
};

}  // namespace raksha::frontends::sql

#endif  // SRC_FRONTENDS_SQL_ID_NAME_AND_STRING_TEST_H_
