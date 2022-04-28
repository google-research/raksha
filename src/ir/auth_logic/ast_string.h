
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
    std::variant<datalog::Predicate, BaseFact, Fact, ConditionalAssertion,
                 Assertion, Argument>;

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
          [](BaseFact basefact) {
            return std::visit(raksha::utils::overloaded{
                                  [](datalog::Predicate predicate) {
                                    return ToString(predicate);
                                  },
                                  [](Attribute attribute) {
                                    return absl::StrJoin(
                                        {attribute.principal().name(),
                                         ToString(attribute.predicate())},
                                        " ");
                                  },
                                  [](CanActAs can_act_as) {
                                    return absl::StrJoin(
                                        {can_act_as.left_principal().name(),
                                         can_act_as.right_principal().name()},
                                        " canActAs ");
                                  }},
                              basefact.GetValue());
          },
          [](Fact fact) {
            if (fact.delegation_chain().empty()) {
              return ToString(fact.base_fact());
            }
            std::string cansay_string = "";
            for (const Principal& delegatees : fact.delegation_chain()) {
              cansay_string =
                  absl::StrJoin({delegatees.name(), cansay_string}, " canSay ");
            }
            return absl::StrJoin({cansay_string, ToString(fact.base_fact())},
                                 " ");
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
          [](Argument argument) {
            return absl::StrJoin(
                {argument.argument_name(), argument.argument_type().name()},
                " : ");
          }},
      Node);
}

}  // namespace raksha::ir::auth_logic
