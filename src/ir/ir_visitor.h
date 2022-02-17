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
#ifndef SRC_IR_IR_VISITOR_H_
#define SRC_IR_IR_VISITOR_H_

namespace raksha::ir {

class Module;
class Block;
class Operation;

// An interface for the visitor class. We will also pass in the `Derived` class
// as template argument if we want to support CRTP at a later point.
template <typename Derived>
class IRVisitor {
 public:
  virtual ~IRVisitor() {}
  virtual void Visit(const Module& module) = 0;
  virtual void Visit(const Block& operation) = 0;
  virtual void Visit(const Operation& operation) = 0;
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_VISITOR_H_
