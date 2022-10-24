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

#include "ast_string.h"

#include "src/common/logging/logging.h"

namespace raksha::ir::auth_logic {

namespace {
template <typename T>
std::vector<std::string> ToStrings(const std::vector<T> nodes) {
  return utils::MapIter<std::string>(
      nodes, [](const T node) { return ToString(node); });
}
}  // namespace

std::vector<std::string> binary_operations = {"<", ">", "=", "!=", "<=", ">="};

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
            switch (argumentType.kind()) {
              case datalog::ArgumentType::Kind::kCustom:
                return absl::StrCat(argumentType.name(), "::Custom");
              case datalog::ArgumentType::Kind::kNumber:
                return std::string("Number");
              case datalog::ArgumentType::Kind::kPrincipal:
                return std::string("Principal");
              default:
                CHECK(false) << "Unexpected datalog argument type in ToString";
            }
          },
          [](datalog::Argument argument) {
            return absl::StrCat(argument.argument_name(), " : ",
                                ToString(argument.argument_type()));
          },
          [](datalog::RelationDeclaration relationDeclaration) {
            return absl::StrCat(
                ".decl ",
                relationDeclaration.is_attribute() ? "attribute " : "",
                relationDeclaration.relation_name(), "(",
                absl::StrJoin(ToStrings(relationDeclaration.arguments()), ", "),
                ")");
          },
          [](Principal principal) { return principal.name(); },
          [](Attribute attribute) {
            return absl::StrCat(ToString(attribute.principal()), " ",
                                ToString(attribute.predicate()));
          },
          [](CanActAs canActAs) {
            return absl::StrCat(ToString(canActAs.left_principal()),
                                " canActAs ",
                                ToString(canActAs.right_principal()));
          },
          [](BaseFact basefact) {
            return std::visit(
                raksha::utils::overloaded{
                    [](datalog::Predicate predicate) {
                      return ToString(predicate);
                    },
                    [](Attribute attribute) { return ToString(attribute); },
                    [](CanActAs can_act_as) { return ToString(can_act_as); }},
                basefact.GetValue());
          },
          [](Fact fact) {
            std::string cansay_string = "";
            for (const Principal& delegatees : fact.delegation_chain()) {
              cansay_string =
                  absl::StrJoin({delegatees.name(), cansay_string}, " canSay ");
            }
            return absl::StrCat(cansay_string, ToString(fact.base_fact()));
          },
          [](ConditionalAssertion conditional_assertion) {
            return absl::StrJoin(
                {ToString(conditional_assertion.lhs()),
                 absl::StrJoin(ToStrings(conditional_assertion.rhs()), ", ")},
                " :- ");
          },
          [](Assertion assertion) {
            return std::visit(
                raksha::utils::overloaded{
                    [](Fact fact) { return absl::StrCat(ToString(fact), "."); },
                    [](ConditionalAssertion conditional_assertion) {
                      return absl::StrCat(ToString(conditional_assertion), ".");
                    }},
                assertion.GetValue());
          },
          [](SaysAssertion saysAssertion) {
            return absl::StrCat(
                ToString(saysAssertion.principal()), " says {\n",
                absl::StrJoin(ToStrings(saysAssertion.assertions()), "\n"),
                "}");
          },
          [](Query query) {
            return absl::StrCat(query.name(), " = ",
                                ToString(query.principal()), " says ",
                                ToString(query.fact()), "?");
          },
          [](Program program) {
            return absl::StrJoin(
                {absl::StrJoin(ToStrings(program.relation_declarations()),
                               "\n"),
                 absl::StrJoin(ToStrings(program.says_assertions()), "\n"),
                 absl::StrJoin(ToStrings(program.queries()), "\n")},
                "\n");
          }},
      Node);
}

}  // namespace raksha::ir::auth_logic