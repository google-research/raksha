/*
 * Copyright 2022 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SRC_IR_AUTH_LOGIC_SOUFFLE_EMITTER_H_
#define SRC_IR_AUTH_LOGIC_SOUFFLE_EMITTER_H_

#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "src/common/logging/logging.h"
#include "src/common/utils/map_iter.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {

class SouffleEmitter {
 public:
  static std::string EmitProgram(const datalog::Program& program) {
    SouffleEmitter emitter;
    std::string body = emitter.EmitProgramBody(program);
    std::string outputs = emitter.EmitOutputs(program);

    std::string declarations = emitter.EmitRelationDeclarations(program);
    std::string type_declarations = emitter.EmitTypeDeclarations(program);
    return absl::StrCat(std::move(type_declarations), "\n",
                        std::move(declarations), "\n", std::move(body), "\n",
                        std::move(outputs));
  }

  static const absl::flat_hash_set<std::string>& GetRelationsToNotDeclare() {
    static const auto* const kRelationsToNotDeclare =
        new absl::flat_hash_set<std::string>{
            "Principal",    "Tag",
            "AccessPath",   "says_isAccessPath",
            "says_isTag",   "says_isPrincipal",
            "says_ownsTag", "says_ownsAccessPath",
            "says_hasTag",  "says_removeTag",
            "says_may",     "says_will",
            "isAccessPath", "isTag",
            "isPrincipal",  "Operation"};
    return *kRelationsToNotDeclare;
  }

 private:
  SouffleEmitter() = default;

  // This function produces a normalized version of predicates to
  // be used when generating declarations of the predicates. It replaces
  // argument names with generic numbered ones. It is applied
  // to predicates before they are added to the set of declarations so that
  // there are no duplicate delcarations (which would otherwise happen
  // whenever a predicate is referenced more than once with different
  // arguments).
  // To prevent instances of negated and non-negated uses of the predicate
  // from generating two declarations, the sign here is always positive.
  datalog::Predicate PredToDeclaration(const datalog::Predicate& predicate) {
    int i = 0;
    return datalog::Predicate(
        predicate.name(),
        utils::MapIter<std::string>(predicate.args(),
                                    [i](const std::string& arg) mutable {
                                      return absl::StrCat("x",
                                                          std::to_string(i++));
                                    }),
        datalog::kPositive);
  }

  std::string EmitPredicate(const datalog::Predicate& predicate) {
    static const absl::flat_hash_set<std::string> comparison_operators = {
        {"<"}, {">"}, {"="}, {"!="}, {"<="}, {">="}};
    // Rvalue rules which are represented as datalog::Predicates in AST, we use
    // infix notation while translating to datalog.
    // Ex: "<"(number1, number2) -> number1 < number2.
    if (comparison_operators.find(predicate.name()) !=
        comparison_operators.end()) {
      CHECK(predicate.args().size() == 2);
      return absl::StrCat(predicate.args().front(), predicate.name(),
                          predicate.args().back());
    }
    // Whenever a new predicate is encountered, it is added to the set of
    // declarations (which does not include duplicates because it is a set).
    declarations_.insert(PredToDeclaration(predicate));
    return absl::StrCat(predicate.sign() == datalog::kNegated ? "!" : "",
                        predicate.name(), "(",
                        absl::StrJoin(predicate.args(), ", "), ")");
  }

  std::string EmitAssertion(const datalog::Rule& cond_assertion) {
    std::vector rhs_translated = utils::MapIter<std::string>(
        cond_assertion.rhs(),
        [this](const datalog::Predicate& arg) { return EmitPredicate(arg); });
    return absl::StrCat(
        EmitPredicate(cond_assertion.lhs()),
        (rhs_translated.size() == 0)
            ? ""
            : absl::StrCat(" :- ", absl::StrJoin(rhs_translated, ", ")),
        ".");
  }

  std::string EmitProgramBody(const datalog::Program& program) {
    return absl::StrJoin(
        utils::MapIter<std::string>(
            program.rules(),
            [this](const datalog::Rule& astn) { return EmitAssertion(astn); }),
        "\n");
  }

  std::string EmitOutputs(const datalog::Program& program) {
    return absl::StrJoin(program.outputs(), "\n",
                         [](std::string* out, const std::string& prog_out) {
                           return absl::StrAppend(out, ".output ", prog_out);
                         });
  }

  std::string EmitDeclaration(const datalog::Predicate& predicate) {
    std::string arguments = absl::StrJoin(
        predicate.args(), ", ", [](std::string* out, const std::string& arg) {
          return absl::StrAppend(out, arg, ": symbol");
        });
    return absl::StrCat(".decl ", predicate.name(), "(", arguments, ")");
  }

  std::string EmitArguments(const std::vector<datalog::Argument>& arguments) {
    return absl::StrJoin(
        arguments, ", ", [](std::string* out, datalog::Argument argument) {
          return absl::StrAppend(out, argument.argument_name(), " : ",
                                 argument.argument_type().name());
        });
  }

  std::string EmitRelationDeclarations(const datalog::Program& program) {
    std::vector<std::string> declaration_strings;
    for (const auto& declaration : program.relation_declarations()) {
      CHECK(!GetRelationsToNotDeclare().empty());
      if (GetRelationsToNotDeclare().find(declaration.relation_name()) !=
          GetRelationsToNotDeclare().end())
        continue;
      declaration_strings.push_back(
          absl::StrCat(".decl ", declaration.relation_name(), "(",
                       EmitArguments(declaration.arguments()), ")"));
    }
    std::sort(declaration_strings.begin(), declaration_strings.end());
    return absl::StrJoin(declaration_strings, "\n");
  }

  std::string EmitTypeDeclarations(const datalog::Program& program) {
    absl::flat_hash_set<std::string> type_names;
    for (const auto& declaration : program.relation_declarations()) {
      for (const auto& argument : declaration.arguments()) {
        if (argument.argument_type().kind() !=
            datalog::ArgumentType::Kind::kCustom)
          continue;
        if (GetRelationsToNotDeclare().find(argument.argument_type().name()) !=
            GetRelationsToNotDeclare().end())
          continue;
        type_names.insert(absl::StrCat(
            ".type ", argument.argument_type().name(), " <: symbol"));
      }
    }
    // TODO (#633) work around till we add number types in C++ version.
    type_names.insert(absl::StrCat(".type Number", " <: symbol"));
    std::vector<absl::string_view> sorted_type_names(type_names.begin(),
                                                     type_names.end());
    std::sort(sorted_type_names.begin(), sorted_type_names.end());
    return absl::StrJoin(sorted_type_names, "\n");
  }

  absl::flat_hash_set<datalog::Predicate> declarations_;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_SOUFFLE_EMITTER_H_
