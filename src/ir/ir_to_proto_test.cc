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

#include "src/ir/ir_to_proto.h"

#include <string>

#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "src/common/testing/gtest.h"
#include "src/ir/ir_printer.h"
#include "src/ir/proto/raksha_ir.pb.h"
#include "src/ir/proto_to_ir.h"
#include "src/parser/ir/ir_parser.h"

namespace raksha::ir {

class IrToProtoTest : public testing::TestWithParam<std::string_view> {};

TEST_P(IrToProtoTest, RoundTripPreservesAllInformation) {
  const auto input_program_text = GetParam();
  IrProgramParser ir_parser;
  // TODO(#615): IRPrinter cannot take a const SsaNames.
  /*const*/ auto parse_result = ir_parser.ParseProgram(input_program_text);
  const Module& program = *parse_result.module;
  const IRContext& context1 = *parse_result.context;
  /*const*/ auto& ssa_names = *parse_result.ssa_names;

  // Check round trip toString (sanity check to ensure later checks are sound).
  EXPECT_EQ(IRPrinter::ToString(program, ssa_names), input_program_text);

  proto::IrTranslationUnit orig_translation_unit_proto =
      IRToProto::Convert(context1, program);

  IRContext context2;
  ProtoToIR::Result recovered = ProtoToIR::Convert(context2, orig_translation_unit_proto);
  const std::string result = IRPrinter::ToString(*recovered.module, *recovered.ssa_names);
  EXPECT_EQ(result, input_program_text);

  proto::IrTranslationUnit recovered_translation_unit_proto =
      IRToProto::Convert(context1, *recovered.module);

  google::protobuf::util::MessageDifferencer differencer;
  std::string difference = "<Compare not yet run>";
  differencer.ReportDifferencesToString(&difference);
  ASSERT_TRUE(differencer.Compare(orig_translation_unit_proto,
                                  recovered_translation_unit_proto))
      << "RECOVERED DATA DID NOT MATCH!\n"
      << difference;
}

INSTANTIATE_TEST_SUITE_P(
    IrToProtoTest, IrToProtoTest,
    ::testing::Values(
        R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus []()
  }  // block b1
}  // module m0
)",
        R"(module m0 {
  block b0 {
    %0 = core.plus []()
  }  // block b0
  block b1 {
    %1 = core.plus [access: "private", transform: "no"](<<ANY>>, <<ANY>>)
  }  // block b1
}  // module m0
)"));
// TODO(#596): Add tests for IR with block arguments and returns.
// TODO(#605): Add tests for IR with out of order declarations.
}  // namespace raksha::ir
