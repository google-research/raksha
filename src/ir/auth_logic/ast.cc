//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include "src/ir/auth_logic/ast.h"

namespace raksha::ir::auth_logic {

  // AstVerbPhrase
  std::unique_ptr<AstVerbPhrase>
  AstVerbPhrase::CreateVerbPhrase(AstPredicate pred) {
    return std::unique_ptr<AstVerbPhrase>(new AstVerbPhrase(std::move(pred)));
  }

  std::unique_ptr<AstVerbPhrase>
  AstVerbPhrase::CreateVerbPhrase(AstPrincipal prin) {
    return std::unique_ptr<AstVerbPhrase>(new AstVerbPhrase(std::move(prin)));
  }

  // AstFlatFact
  std::unique_ptr<AstFlatFact>
  AstFlatFact::CreateFlatFact(AstPrincipal prin,
      AstVerbPhrase verb_phrase) {
    VerbPhraseFlatFact* verb_phrase_flat_fact = new VerbPhraseFlatFact(
        prin, verb_phrase);
    return std::unique_ptr<AstFlatFact>(new AstFlatFact(
      std::move(*verb_phrase_flat_fact)));
  }

  std::unique_ptr<AstFlatFact>
  AstFlatFact::CreateFlatFact(AstPredicate pred) {
    return std::unique_ptr<AstFlatFact>(new AstFlatFact(std::move(pred)));
  }

  // AstFact
  std::unique_ptr<AstFact>
  AstFact::CreateFact(AstFlatFact flat_fact) {
    return std::unique_ptr<AstFact>(new AstFact(std::move(flat_fact)));
  }
  
  std::unique_ptr<AstFact>
  AstFact::CreateFact(AstPrincipal prin, AstFact* fact) {
    CanSayFact* can_say_fact = new CanSayFact(std::move(prin), std::move(fact));
    return std::unique_ptr<AstFact>(new AstFact(std::move(*can_say_fact)));
  }

  // AstAssertion
  std::unique_ptr<AstAssertion>
  AstAssertion::CreateAssertion(AstFact fact) {
    return std::unique_ptr<AstAssertion>(new AstAssertion(std::move(fact)));
  }

  std::unique_ptr<AstAssertion>
  AstAssertion::CreateAssertion(AstFact lhs, std::vector<AstFlatFact> rhs) {
    CondAssertion* cond_assertion = new CondAssertion(std::move(lhs),
        std::move(rhs));
    return std::unique_ptr<AstAssertion>(new AstAssertion(
        std::move(*cond_assertion)));
  }

}
