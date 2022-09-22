
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
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/common/utils/overloaded.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {
std::vector<std::string> binary_operations = {"<", ">", "=", "!=", "<=", ">="};

using AstNodeVariantType =
    std::variant<datalog::Predicate, datalog::ArgumentType,
    datalog::Argument, datalog::RelationDeclaration,
    Principal, Attribute, CanActAs,
    BaseFact, Fact, ConditionalAssertion,
    Assertion, SaysAssertion, Query, Program>;

std::string ToString(AstNodeVariantType Node) {
  return std::visit(
      raksha::utils::overloaded{
          [](datalog::Predicate predicate) {
            if (std::find(binary_operations.begin(), binary_operations.end(),
                          predicate.name()) != binary_operations.end()) {
              return absl::StrCat(predicate.args().front(), " ",
                                  predicate.name(), " ",
                                  predicate.args().back());
            }
            return absl::StrCat(
                predicate.sign() == datalog::Sign::kNegated ? "!" : "",
                predicate.name(), "(", absl::StrJoin(predicate.args(), ", "),
                ")");
          },
          [](datalog::ArgumentType argumentType) {
            switch(argumentType.kind()) {
              case datalog::ArgumentType::Kind::kCustom:
                return absl::StrCat(argumentType.name(), "::CustomType");
              case datalog::ArgumentType::Kind::kNumber:
                return std::string("NumberType");
              case datalog::ArgumentType::Kind::kPrincipal:
                return std::string("PrincipalType");
            }
          },
          [](datalog::Argument argument) {
            return absl::StrCat(
                argument.argument_name(), " : ",
                ToString(argument.argument_type()));
          },
          [](datalog::RelationDeclaration relationDeclaration) {
            std::vector<std::string> arg_strings;
            arg_strings.reserve(relationDeclaration.arguments().size());
            for (const datalog::Argument& arg : relationDeclaration.arguments()) {
              arg_strings.push_back(ToString(arg));
            }
            return absl::StrCat(".decl ",
              relationDeclaration.is_attribute() ? "attribute " : "",
              relationDeclaration.relation_name(), "(",
                                absl::StrJoin(arg_strings, ", "), ")");
          },
          [](Principal principal) {
            return principal.name();
          },
          [](Attribute attribute) {
               return absl::StrCat(ToString(attribute.principal()),
                " ",
                ToString(attribute.predicate()));
          },
          [](CanActAs canActAs) {
            return absl::StrCat(ToString(canActAs.left_principal()),
              " canActAs ", ToString(canActAs.right_principal()));
          },
          [](BaseFact basefact) {
            return std::visit(raksha::utils::overloaded{
                                  [](datalog::Predicate predicate) {
                                    return absl::StrCat("BaseFact(",
                                      ToString(predicate), ")");
                                  },
                                  [](Attribute attribute) {
                                    return absl::StrCat("BaseFact(",
                                      ToString(attribute), ")");
                                  },
                                  [](CanActAs can_act_as) {
                                    return absl::StrCat("BaseFact(",
                                      ToString(can_act_as), ")");
                                  }},
                              basefact.GetValue());
          },
          [](Fact fact) {
            std::string cansay_string = "";
            for (const Principal& delegatees : fact.delegation_chain()) {
              cansay_string =
                  absl::StrJoin({delegatees.name(), cansay_string}, " canSay ");
            }
            return absl::StrCat("Fact(", cansay_string,
              ToString(fact.base_fact()), ")");
          },
          [](ConditionalAssertion conditional_assertion) {
            std::string lhs = ToString(conditional_assertion.lhs());
            std::vector<std::string> rhs;
            for (const BaseFact& base_fact : conditional_assertion.rhs()) {
              rhs.push_back(ToString(base_fact));
            }
            return absl::StrJoin({lhs, absl::StrJoin(rhs, ",")}, " :- ");
          },
          [](Assertion assertion) {
            return std::visit(
                raksha::utils::overloaded{
                    [](Fact fact) { return ToString(fact); },
                    [](ConditionalAssertion conditional_assertion) {
                      return ToString(conditional_assertion);
                    }},
                assertion.GetValue());
          },
          [](SaysAssertion saysAssertion) {
            std::vector<std::string> assertion_strings;
            assertion_strings.reserve(saysAssertion.assertions().size());
            for (const Assertion& assertion : saysAssertion.assertions()) {
              assertion_strings.push_back(ToString(assertion));
            }
            return absl::StrCat(ToString(saysAssertion.principal()), "says {\n",
                        absl::StrJoin(assertion_strings, "\n"), "}");
          },
          [](Query query) {
            return absl::StrCat("Query(",
              query.name(), ToString(query.principal()),
                        ToString(query.fact()), ")");
          },
          [](Program program) {
            std::vector<std::string> relation_decl_strings;
            relation_decl_strings.reserve(program.relation_declarations().size());
            for (const datalog::RelationDeclaration& rel_decl :
                 program.relation_declarations()) {
              relation_decl_strings.push_back(ToString(rel_decl));
            }
            std::vector<std::string> says_assertion_strings;
            says_assertion_strings.reserve(program.says_assertions().size());
            for (const SaysAssertion& says_assertion :
                program.says_assertions()) {
              says_assertion_strings.push_back(ToString(says_assertion));
            }
            std::vector<std::string> query_strings;
            query_strings.reserve(program.queries().size());
            for (const Query& query : program.queries()) {
              query_strings.push_back(ToString(query));
            }
            return absl::StrCat("Program(\n",
                                absl::StrJoin(relation_decl_strings, "\n"),
                                absl::StrJoin(says_assertion_strings, "\n"),
                                absl::StrJoin(query_strings, "\n"), ")");
          }},
      Node);
}

}  // namespace raksha::ir::auth_logic
