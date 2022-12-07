//-------------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------

#include "src/policy/parser.h"
#include "src/common/logging/logging.h"
#include "src/policy/policy_cc_generator_grammar.inc/policy_cc_generator/PolicyLexer.h"
#include "src/policy/policy_cc_generator_grammar.inc/policy_cc_generator/PolicyParser.h"

namespace raksha::policy {

class ErrorStrategy : public antlr4::DefaultErrorStrategy {
  void reportError(antlr4::Parser* parser,
                   const antlr4::RecognitionException& e) override {
    parser->notifyErrorListeners("error");
  }
};
bool ParseProgramIsValid(absl::string_view prog_text) {
  // Provide the input text in a stream
  antlr4::ANTLRInputStream input(prog_text);
  // Creates a lexer from input
  policy_cc_generator::PolicyLexer lexer(&input);
  // Creates a token stream from lexer
  antlr4::CommonTokenStream tokens(&lexer);
  // Creates a parser from the token stream
  policy_cc_generator::PolicyParser parser(&tokens);
  auto error_strategy = std::make_shared<ErrorStrategy>();
  parser.setErrorHandler(error_strategy);
  parser.policy();
  return parser.getNumberOfSyntaxErrors() == 0;
}

}  // namespace raksha::policy
