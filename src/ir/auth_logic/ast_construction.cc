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

#include <forward_list>
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
#include "src/policy/auth_logic/auth_logic_cc_generator_grammar.inc/auth_logic_cc_generator/AuthLogicLexer.h"
#include "src/policy/auth_logic/auth_logic_cc_generator_grammar.inc/auth_logic_cc_generator/AuthLogicParser.h"

namespace raksha::ir::auth_logic {

using auth_logic_cc_generator::AuthLogicLexer;
using auth_logic_cc_generator::AuthLogicParser;

static Principal ConstructPrincipal(
    AuthLogicParser::PrincipalContext& principal_context) {
  return Principal(principal_context.ID()->getText());
}

static datalog::Predicate ConstructPredicate(
    AuthLogicParser::PredicateContext& predicate_context) {
  std::vector<std::string> args = utils::MapIter<std::string>(
      predicate_context.pred_arg(),
      [](auto* pred_arg) { return pred_arg->getText(); });
  datalog::Sign sign = predicate_context.NEG() == nullptr
                           ? datalog::Sign::kPositive
                           : datalog::Sign::kNegated;
  return datalog::Predicate(predicate_context.ID()->getText(), std::move(args),
                            sign);
}

static BaseFact ConstructVerbphrase(
    Principal left_principal,
    AuthLogicParser::VerbphraseContext& verbphrase_context) {
  if (auto* predphrase_context =
          dynamic_cast<AuthLogicParser::PredphraseContext*>(
              &verbphrase_context)) {  // PredicateContext
    return BaseFact(Attribute(
        left_principal,
        ConstructPredicate(*CHECK_NOTNULL(predphrase_context->predicate()))));
  }
  auto& act_as_phrase_context =
      *CHECK_NOTNULL(dynamic_cast<AuthLogicParser::ActsAsPhraseContext*>(
          &verbphrase_context));  // ActsAsPhraseContext
  return BaseFact(CanActAs(
      left_principal,
      ConstructPrincipal(*CHECK_NOTNULL(act_as_phrase_context.principal()))));
}

static BaseFact ConstructFlatFact(
    AuthLogicParser::FlatFactContext& flat_fact_context) {
  if (auto* prin_fact_context =
          dynamic_cast<AuthLogicParser::PrinFactContext*>(&flat_fact_context)) {
    return ConstructVerbphrase(
        ConstructPrincipal(*CHECK_NOTNULL(prin_fact_context->principal())),
        *CHECK_NOTNULL(prin_fact_context->verbphrase()));
  }
  auto& pred_fact_context =
      *CHECK_NOTNULL(dynamic_cast<AuthLogicParser::PredFactContext*>(
          &flat_fact_context));  // predFactContext
  return BaseFact(
      ConstructPredicate(*CHECK_NOTNULL(pred_fact_context.predicate())));
}

static BaseFact ConstructRvalue(
    AuthLogicParser::RvalueContext& rvalue_context) {
  if (auto* flat_fact_rvalue_context =
          dynamic_cast<AuthLogicParser::FlatFactRvalueContext*>(
              &rvalue_context)) {
    return ConstructFlatFact(
        *CHECK_NOTNULL(flat_fact_rvalue_context->flatFact()));
  }
  auto& binop_rvalue_context = *CHECK_NOTNULL(
      dynamic_cast<AuthLogicParser::BinopRvalueContext*>(&rvalue_context));
  std::vector<std::string> args = utils::MapIter<std::string>(
      binop_rvalue_context.pred_arg(),
      [](auto* pred_arg) { return pred_arg->getText(); });
  return BaseFact(datalog::Predicate(binop_rvalue_context.binop()->getText(),
                                     std::move(args),
                                     datalog::Sign::kPositive));
}

static Fact ConstructFact(std::forward_list<Principal> delegation_chain,
                          AuthLogicParser::FactContext& fact_context) {
  if (auto* flat_fact_fact_context =
          dynamic_cast<AuthLogicParser::FlatFactFactContext*>(
              &fact_context)) {  // FlatFactFactContext
    return Fact(
        std::move(delegation_chain),
        ConstructFlatFact(*CHECK_NOTNULL(flat_fact_fact_context->flatFact())));
  }
  auto& can_say_fact_context = *CHECK_NOTNULL(
      dynamic_cast<AuthLogicParser::CanSayFactContext*>(&fact_context));
  delegation_chain.push_front(
      ConstructPrincipal(*CHECK_NOTNULL(can_say_fact_context.principal())));
  return ConstructFact(std::move(delegation_chain),
                       *CHECK_NOTNULL(can_say_fact_context.fact()));
}

static Query ConstructQuery(AuthLogicParser::QueryContext& query_context) {
  return Query(query_context.ID()->getText(),
               ConstructPrincipal(*CHECK_NOTNULL(query_context.principal())),
               ConstructFact({}, *CHECK_NOTNULL(query_context.fact())));
}

static Assertion ConstructAssertion(
    AuthLogicParser::AssertionContext& assertion_context) {
  if (auto* fact_assertion_context =
          dynamic_cast<AuthLogicParser::FactAssertionContext*>(
              &assertion_context)) {
    return Assertion(
        ConstructFact({}, *CHECK_NOTNULL(fact_assertion_context->fact())));
  }
  auto& horn_clause_assertion_context =
      *CHECK_NOTNULL(dynamic_cast<AuthLogicParser::HornClauseAssertionContext*>(
          &assertion_context));
  std::vector<BaseFact> base_facts = utils::MapIter<BaseFact>(
      horn_clause_assertion_context.rvalue(),
      [](AuthLogicParser::RvalueContext* rvalue_context) {
        return ConstructRvalue(*CHECK_NOTNULL(rvalue_context));
      });
  return Assertion(ConditionalAssertion(
      ConstructFact({}, *CHECK_NOTNULL(horn_clause_assertion_context.fact())),
      std::move(base_facts)));
}

static SaysAssertion ConstructSaysAssertion(
    AuthLogicParser::SaysAssertionContext& says_assertion_context) {
  if (auto* says_single_context =
          dynamic_cast<AuthLogicParser::SaysSingleContext*>(
              &says_assertion_context)) {
    std::vector<Assertion> assertions;
    assertions.push_back(
        ConstructAssertion(*CHECK_NOTNULL(says_single_context->assertion())));
    return SaysAssertion(
        ConstructPrincipal(*CHECK_NOTNULL(says_single_context->principal())),
        std::move(assertions));
  }
  auto& says_multi_context =
      *CHECK_NOTNULL(dynamic_cast<AuthLogicParser::SaysMultiContext*>(
          &says_assertion_context));
  std::vector<Assertion> assertions = utils::MapIter<Assertion>(
      says_multi_context.assertion(),
      [](AuthLogicParser::AssertionContext* assertion_context) {
        return ConstructAssertion(*CHECK_NOTNULL(assertion_context));
      });
  return SaysAssertion(
      ConstructPrincipal(*CHECK_NOTNULL(says_multi_context.principal())),
      std::move(assertions));
}

static Program ConstructProgram(
    AuthLogicParser::ProgramContext& program_context) {
  std::vector<SaysAssertion> says_assertions = utils::MapIter<SaysAssertion>(
      program_context.saysAssertion(),
      [](AuthLogicParser::SaysAssertionContext* says_assertion_context) {
        return ConstructSaysAssertion(*CHECK_NOTNULL(says_assertion_context));
      });
  std::vector<Query> queries = utils::MapIter<Query>(
      program_context.query(),
      [](AuthLogicParser::QueryContext* query_context) {
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
