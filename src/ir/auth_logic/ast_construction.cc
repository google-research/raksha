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

#include "src/ir/auth_logic/ast_construction.h"

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "src/common/logging/logging.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"
#include "src/policy/auth_logic/auth_logic_cc_generator_grammar.inc/AuthLogicLexer.h"
#include "src/policy/auth_logic/auth_logic_cc_generator_grammar.inc/AuthLogicParser.h"


namespace raksha::ir::auth_logic {

static Principal ConstructPrincipal(
    AuthLogicParser::PrincipalContext& context) {
  return Principal(context.ID()->getText());
}

static datalog::Predicate ConstructPredicate(
    AuthLogicParser::PredicateContext& context) {
  std::vector<std::string> args = utils::MapIter<std::string>(
      context.pred_arg(),
      [](auto* pred_arg) { return pred_arg->ID()->getText(); });
  datalog::Sign sign = context.NEG() == nullptr ? datalog::Sign::kPositive
                                                : datalog::Sign::kNegated;
  return datalog::Predicate(context.ID()->getText(), args, sign);
}

static BaseFact ConstructVerbphrase(
    Principal left_principal, AuthLogicParser::VerbphraseContext& context) {
  if (auto* ctx = dynamic_cast<AuthLogicParser::PredphraseContext*>(
          &context)) {  // PredicateContext
    return BaseFact(Attribute(
        left_principal, ConstructPredicate(*CHECK_NOTNULL(ctx->predicate()))));
  }
  auto& ctx =
      *CHECK_NOTNULL(dynamic_cast<AuthLogicParser::ActsAsPhraseContext*>(
          &context));  // ActsAsPhraseContext
  return BaseFact(CanActAs(
      left_principal, ConstructPrincipal(*CHECK_NOTNULL(ctx.principal()))));
}

static BaseFact ConstructFlatFact(AuthLogicParser::FlatFactContext& context) {
  if (auto* ctx = dynamic_cast<AuthLogicParser::PrinFactContext*>(&context)) {
    return ConstructVerbphrase(
        ConstructPrincipal(*CHECK_NOTNULL(ctx->principal())),
        *CHECK_NOTNULL(ctx->verbphrase()));
  }
  auto& ctx = *CHECK_NOTNULL(dynamic_cast<AuthLogicParser::PredFactContext*>(
      &context));  // predFactContext
  return BaseFact(ConstructPredicate(*CHECK_NOTNULL(ctx.predicate())));
}

static Fact ConstructFact(AuthLogicParser::FactContext& context) {
  if (auto* flat_fact_fact_ctx =
          dynamic_cast<AuthLogicParser::FlatFactFactContext*>(
              &context)) {  // FlatFactFactContext
    return Fact(
        ConstructFlatFact(*CHECK_NOTNULL(flat_fact_fact_ctx->flatFact())));
  }
  auto& ctx = *CHECK_NOTNULL(
      dynamic_cast<AuthLogicParser::CanSayFactContext*>(&context));
  return ConstructFact(*CHECK_NOTNULL(ctx.fact()));
}

static Query ConstructQuery(AuthLogicParser::QueryContext& context) {
  return Query(context.ID()->getText(),
               ConstructPrincipal(*CHECK_NOTNULL(context.principal())),
               ConstructFact(*CHECK_NOTNULL(context.fact())));
}

static Program ConstructProgram(AuthLogicParser::ProgramContext& context) {
  std::vector<SaysAssertion> says_assertions;
  std::vector<Query> queries = utils::MapIter<Query>(
      context.query(), [](AuthLogicParser::QueryContext* query_context) {
        return ConstructQuery(*CHECK_NOTNULL(query_context));
      });
  return Program(std::move(says_assertions), std::move(queries));
}

/// This function produces an abstract syntax tree (AST) rooted with a program
/// node when given the textual representation of a program.
Program ParseProgram(absl::string_view prog_text) {
  // Provide the input text in a stream
  antlr4::ANTLRInputStream input(prog_text);
  // Creates a lexer from input
  AuthLogicLexer lexer(&input);
  // Creates a token stream from lexer
  antlr4::CommonTokenStream tokens(&lexer);
  // Creates a parser from the token stream
  AuthLogicParser parser(&tokens);
  // program_context points to the root of the parse tree
  AuthLogicParser::ProgramContext& program_context =
      *CHECK_NOTNULL(parser.program());
  return ConstructProgram(program_context);
}

}  // namespace raksha::ir::auth_logic
