//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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
#ifndef SRC_IR_AUTH_LOGIC_AST_VISITOR_H_
#define SRC_IR_AUTH_LOGIC_AST_VISITOR_H_

#include "src/common/utils/types.h"

namespace raksha::ir::auth_logic {

class Principal;
class Attribute;
class CanActAs;
class BaseFact;
class Fact;
class ConditionalAssertion;
class Assertion;
class SaysAssertion;
class Query;
class Program;

enum AstNodeMutability : bool { Mutable = false, Immutable = true };

template <typename Derived, typename Result = Unit,
          AstNodeMutability IsConst = AstNodeMutability::Immutable>
class AuthLogicAstVisitor {
 public:
  virtual ~AuthLogicAstVisitor() {}
  virtual Result Visit(CopyConst<IsConst, Principal>& principal) = 0;
  virtual Result Visit(CopyConst<IsConst, Attribute>& attribute) = 0;
  virtual Result Visit(CopyConst<IsConst, CanActAs>& canActAs) = 0;
  virtual Result Visit(CopyConst<IsConst, BaseFact>& baseFact) = 0;
  virtual Result Visit(CopyConst<IsConst, Fact>& fact) = 0;
  virtual Result Visit(
      CopyConst<IsConst, ConditionalAssertion>& conditionalAssertion) = 0;
  virtual Result Visit(CopyConst<IsConst, Assertion>& assertion) = 0;
  virtual Result Visit(CopyConst<IsConst, SaysAssertion>& saysAssertion) = 0;
  virtual Result Visit(CopyConst<IsConst, Query>& query) = 0;
  virtual Result Visit(CopyConst<IsConst, Program>& program) = 0;
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_VISITOR_H_
