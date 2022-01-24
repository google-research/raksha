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

#ifndef SRC_IR_AUTH_LOGIC_SOUFFLE_EMITTER_H_
#define SRC_IR_AUTH_LOGIC_SOUFFLE_EMITTER_H_

#include <unordered_set>

#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

class SouffleEmitter {
  public:
    std::string EmitProgram(DLIRProgram program) {
      SouffleEmitter emitter;
      std::string body = emitter.EmitProgramBody(program);
      std::string outputs = emitter.EmitOutputs(program);
      std::string decls = emitter.EmitDeclarations();
      return std::move(body) 
        + "\n" 
        + std::move(outputs) 
        + "\n" 
        + std::move(decls);
    }

  private:
    SouffleEmitter() 
      : declarations_(
          std::unordered_set<Predicate, Predicate::HashFunction>()) 
    {};

    // This function produces a normalized version of predicates to
    // be used when generating declarations of the predicates. It replaces
    // argument names with generic numbered ones. It is applied
    // to predicates before they are added to the set of declarations so that
    // there are no duplicate delcarations (which would otherwise happen
    // whenever a predicate is referenced more than once with different
    // arguments).
    // To prevent instances of negated and non-negated uses of the predicate 
    // from generating two declarations, the sign here is always positive.
    Predicate PredToDeclaration(Predicate predicate) {
      std::vector<std::string> decl_args;
      for( int i=0; i < predicate.args().size(); i++ ) {
        decl_args.push_back("x" + std::to_string(i));
      }
      return Predicate(predicate.name(), predicate.args(), kPositive);
    }

    std::string EmitVecSeparated(std::vector<std::string> vec,
        std::string sep) {
      std::string ret = "";
      bool is_first = true;
      for(auto elm : vec) {
        if( !is_first ) { ret += sep; } else { is_first = false; }
        ret += elm;
      }
      return ret;
    }

    std::string EmitPredicate(Predicate predicate) {
      // Whenever a new predicate is encountered, it is added to the set of 
      // declarations (which does not include duplicates because it is a set).
      declarations_.insert(PredToDeclaration(predicate));
      return (predicate.sign() == kNegated ? "!" : "")
          + predicate.name()
          + "(" + EmitVecSeparated(predicate.args(), ", ") + ")";
    }

    std::string EmitAssertionInner(Predicate predicate) {
      return EmitPredicate(predicate) + ".";
    }

    std::string EmitAssertionInner(DLIRCondAssertion cond_assertion) {
      std::string lhs = EmitPredicate(cond_assertion.lhs());
      std::vector<std::string> rhs_translated;
      for(auto arg: cond_assertion.rhs()) {
        rhs_translated.push_back(EmitPredicate(arg));
      }
      std::string rhs = EmitVecSeparated(rhs_translated, ", ");
      return std::move(lhs) + " :- " + std::move(rhs) + ".";
    }

    std::string EmitAssertion(DLIRAssertion assertion) {
      return std::visit(
          [this](auto value) { return EmitAssertionInner(value); },
          assertion.GetValue());
    }

    std::string EmitProgramBody(DLIRProgram program) {
      std::vector<std::string> body_translated;
      for(auto assertion : program.assertions()) {
        body_translated.push_back(EmitAssertion(assertion));
      }
      return EmitVecSeparated(body_translated, "\n");
    }

    std::string EmitOutputs(DLIRProgram program) {
      std::string ret;
      for (auto out : program.outputs()) {
        ret += ".output " + out + "\n";
      }
      return ret;
    }

    std::string EmitDeclaration(Predicate predicate) {
      std::string args = "";
      bool is_first = true;
      for(auto elm : predicate.args()) {
        if(!is_first) { args += ", "; } else { is_first = false; }
        args += elm + ": symbol";
      }
      return ".decl " + predicate.name() + "("
        + args + ")";
    }

    std::string EmitDeclarations() {
      std::vector<std::string> ret;
      for(auto decl : declarations_) {
        ret.push_back(EmitDeclaration(decl));
      }
      return EmitVecSeparated(ret, "\n");
    }

    std::unordered_set<Predicate, Predicate::HashFunction> declarations_;
};

}  // namespace raksha::ir::auth_logic

#endif //SRC_IR_AUTH_LOGIC_SOUFFLE_EMITTER_H_
