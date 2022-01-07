/*
 * Copyright 2021 The Raksha Authors
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

//! This file contains the datalog IR (DLIR) which makes the translation from
//! this authorization logic into datalog simpler.
#ifndef SRC_IR_AUTH_LOGIC_LOWERING_AST_DATALOG_H_
#define SRC_IR_AUTH_LOGIC_LOWERING_AST_DATALOG_H_

#include "ast.h"

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

    static DLIRProgram* Lower(const Program& auth_logic_program) {
      LoweringToDatalogPass* pass = new LoweringToDatalogPass();
      DLIRProgram* dlir_program = pass->ProgToDLIR(auth_logic_program);
      delete pass;
      return dlir_program;
    }

   private:
    LoweringToDatalogPass()
      : fresh_var_count_(0) {}

    int fresh_var_count_;

    // This returns a fresh variable name. Fresh variables are introduced in a
    // few places during the translation.
    std::string FreshVar() {
      return "x__" + std::to_string(fresh_var_count_++);
    }

    // This function makes a copy of a predicate that prepends the name with
    // another string and prepends the list of arguments with additional 
    // arguments. This is used in a few places in the translation, for
    // example, to translate "X says blah(args)" into "says_blah(X, args)".
    Predicate PushOntoPred(std::string modifier,
        std::vector<std::string> new_args, const Predicate& predicate) {
      std::string new_name = modifier + predicate.name();
      new_args.insert(new_args.end(), predicate.args().begin(),
          predicate.args().end());
      Sign sign_copy = predicate.sign();
      return Predicate(new_name, new_args, sign_copy);
    }

    Predicate PushPrin(std::string modifier,
        const Principal& principal, const Predicate& predicate) {
      return PushOntoPred(modifier, {principal.name()}, predicate);
    }

    Predicate AttributeToDLIR(const Attribute& attribute) {
      // If attribute is `X pred(args...)` the following predicate is
      // `pred(X, args...)`
      return PushPrin(std::string(""), attribute.principal(),
          attribute.predicate());
    }

    // This produces an extra rule that needs to be generated during the 
    // translation of "speaker says attribute" into DLIR
    DLIRCondAssertion SpokenAttributeToDLIR(const Principal& speaker,
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
      Predicate generated_lhs = PushPrin(std::string("says_"),
          prin_y, main_predicate);

      Predicate y_can_act_as_x = Predicate(std::string("canActAs"),
          {prin_y.name(), attribute.principal().name()},
          Sign::kPositive);

      Predicate speaker_says_y_can_act_as_x = PushPrin(
          std::string("says_"), speaker, y_can_act_as_x);

      // This is `speaker says X PredX`
      Predicate speaker_says_x_pred = PushPrin(std::string("says_"),
          speaker, main_predicate);

      // This is the full generated rule:
      // `speaker says Y PredX :-
      //    speaker says Y canActAs X, speaker says X PredX`
      return DLIRCondAssertion(generated_lhs,
          {speaker_says_y_can_act_as_x, speaker_says_x_pred});
    }

    DLIRProgram* ProgToDLIR(const Program& program) {
      // TODO / WIP
      return new DLIRProgram({}, {});
    }

  };

} // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_H_
