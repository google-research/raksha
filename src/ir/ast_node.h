
/* Copyright 2022 Google LLC.
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

#ifndef SRC_IR_AST_NODE_H_
#define SRC_IR_AST_NODE_H_

namespace raksha::ir {
// This is the supertype for all nodes in
// `raksha::ir::datalog::program.h` and 
// `raksha::ir::auth_logic::ast.h`
class AstNode {
    public:
    virtual bool operator==(const AstNode& otherNode ) const = 0;
    virtual std::string DebugPrint() const = 0;
    protected:
    AstNode() = default;
};

}; // raksha::ir
#endif // SRC_IR_AST_NODE_H_