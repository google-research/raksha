/*
 * Copyright 2022 The Raksha Authors
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

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.
#ifndef SRC_IR_AUTH_LOGIC_LOWERING_AST_DATALOG_H_
#define SRC_IR_AUTH_LOGIC_LOWERING_AST_DATALOG_H_

#include <utility>  //included for std::pair

#include "absl/strings/str_format.h"
#include "src/ir/auth_logic/ast.h"
#include "src/utils/overloaded.h"

namespace raksha::ir::auth_logic {

// This implements a compiler pass for converting authorization logic
// (represented by ast.h) into the datalog ir (in datalog_ir.h). The only
// public interface to this class is a single function "Lower" which takes
// as input an authorization logic program and returns a datalog program.
//
// (Internally, this is a stateful class because it uses a counter to
// generate fresh names for arguments. The public interface hides this
// implementation detail.)
class LoweringToDatalogPass {
 public:
  static DLIRProgram Lower(const Program& auth_logic_program) {
    return LoweringToDatalogPass().ProgToDLIR(auth_logic_program);
  }

 private:
  LoweringToDatalogPass() : fresh_var_count_(0) {}

  int fresh_var_count_;

  // This returns a fresh variable name. Fresh variables are introduced in a
  // few places during the translation.
  std::string FreshVar() { return "x__" + std::to_string(++fresh_var_count_); }

  // This function makes a copy of a predicate that prepends the name with
  // another string and prepends the list of arguments with additional
  // arguments. This is used in a few places in the translation, for
  // example, to translate "X says blah(args)" into "says_blah(X, args)".
  Predicate PushOntoPred(std::string modifier,
                         std::vector<std::string> new_args,
                         Predicate predicate) {
    std::string new_name = modifier + predicate.name();
    new_args.insert(new_args.end(),
                    std::make_move_iterator(predicate.args().begin()),
                    std::make_move_iterator(predicate.args().end()));
    Sign sign_copy = predicate.sign();
    return Predicate(new_name, new_args, sign_copy);
  }

  Predicate PushPrin(std::string modifier, const Principal& principal,
                     Predicate predicate) {
    return PushOntoPred(modifier, {principal.name()}, predicate);
  }

  Predicate AttributeToDLIR(const Attribute& attribute) {
    // If attribute is `X pred(args...)` the following predicate is
    // `pred(X, args...)`
    return PushPrin(std::string(""), attribute.principal(),
                    attribute.predicate());
  }

  // This is the translation for the attribute case of a base fact.
  // This returns two outputs as a pair:
  // - A predicate, this is the main result of the translation
  // - An additional rule that allows attributes to be passed with canActAs
  // This takes both a speaker and the actual attribute as argument because
  // the speaker is needed to generate the additional rule. Note all base
  // facts have a speaker either explicitly or implicitly:
  // - If it appears on the LHS of an assertion, it explicitly has a speaker
  // - If it appears on the RHS of an assertion, it behaves semantically
  // like has the same speaker as the head of the assertion.
  DLIRAssertion SpokenAttributeToDLIR(const Principal& speaker,
                                      const Attribute& attribute) {
    Predicate main_predicate = AttributeToDLIR(attribute);

    // Attributes interact with "canActAs" because if "Y canActAs X"
    // then Y also picks up X's attributes. We need to generate
    // an additional rule to implement this behavior. If the attribute
    // under translation is `X PredX`, the additional rule is:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X PredX`
    // (Where Y is a fresh variable)
    Principal prin_y = Principal(FreshVar());

    // This is `speaker says Y PredX`
    Predicate generated_lhs =
        PushPrin(std::string("says_"), prin_y, main_predicate);

    Predicate y_can_act_as_x = Predicate(
        std::string("canActAs"), {prin_y.name(), attribute.principal().name()},
        Sign::kPositive);

    Predicate speaker_says_y_can_act_as_x =
        PushPrin(std::string("says_"), speaker, y_can_act_as_x);

    // This is `speaker says X PredX`
    Predicate speaker_says_x_pred =
        PushPrin(std::string("says_"), speaker, main_predicate);

    // This is the full generated rule:
    // `speaker says Y PredX :-
    //    speaker says Y canActAs X, speaker says X PredX`
    return DLIRAssertion(DLIRCondAssertion(
        generated_lhs, {speaker_says_y_can_act_as_x, speaker_says_x_pred}));
  }

  // The second return value represents 0 or 1 newly generated rules, so an
  // option might seem more intuitive. However, the interface that consumes
  // this needs to construct a vector anyway.
  std::pair<Predicate, DLIRAssertion> BaseFactToDLIR(
      const Principal& speaker, const BaseFact& base_fact) {
    return std::visit(
        raksha::utils::overloaded{
            [](Predicate predicate) {
              Predicate dummy_pred =
                  Predicate("pred_name", {"arg1"}, kPositive);
              DLIRAssertion dummy_assertion =
                  DLIRAssertion(DLIRCondAssertion(dummy_pred, {dummy_pred}));
              return std::make_pair(predicate, dummy_assertion);
            },
            [this, &speaker](Attribute attribute) {
              return std::make_pair(AttributeToDLIR(attribute),
                                    SpokenAttributeToDLIR(speaker, attribute));
            },
            [](CanActAs canActAs) {
              Predicate dummy_pred =
                  Predicate("pred_name", {"arg1"}, kPositive);
              DLIRAssertion dummy_assertion =
                  DLIRAssertion(DLIRCondAssertion(dummy_pred, {dummy_pred}));
              return std::make_pair(dummy_pred, dummy_assertion);
            }},
        base_fact.GetValue());
  }

  // This can result in 0 or more new rules because the translation of
  // nested canSayFacts might result in more than 1 rule.
  std::pair<Predicate, std::vector<DLIRAssertion>> FactToDLIR(
      const Principal& speaker, const Fact& fact) {
    // TODO fill in real implementation. This is just a stub.
    Predicate dummy_pred = Predicate("pred_name", {"arg1"}, kPositive);
    DLIRAssertion dummy_assertion =
        DLIRAssertion(DLIRCondAssertion(dummy_pred, {dummy_pred}));
    // This vector needed to be declared before the call to make_pair because
    // template argument inference could not figure it out otherwise.
    std::vector<DLIRAssertion> dummy_assertions = {dummy_assertion};
    return std::make_pair(dummy_pred, dummy_assertions);
  }

  // WIP need to figure out error message about the visitor being
  // non-exhaustive even though it looks exhaustive AFAICT
  // std::vector<DLIRAssertion>
  // SingleSaysAssertionToDLIR(const Principal& speaker,
  //     const Assertion& assertion) {
  //   return std::visit(raksha::utils::overloaded {
  //       [this, &speaker](Fact fact) {
  //         auto[fact_predicate, generated_rules] = FactToDLIR(
  //             speaker, fact);
  //         DLIRAssertion main_assertion = DLIRAssertion(
  //               PushPrin(std::string("says_"), speaker, fact_predicate));
  //         generated_rules.push_back(main_assertion);
  //         return generated_rules;
  //       },
  //       [](ConditionalAssertion conditional_assertion) {
  //         std::vector<DLIRAssertion> dummy_ret = {};
  //         return dummy_ret;
  //       }
  //     }, assertion.GetValue());
  // }

  DLIRProgram ProgToDLIR(const Program& program) {
    // TODO / WIP
    return DLIRProgram({}, {});
  }
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
