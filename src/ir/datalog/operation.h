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

#ifndef SRC_IR_DATALOG_OPERATION_H_
#define SRC_IR_DATALOG_OPERATION_H_

#include <string_view>

#include "src/ir/datalog/attribute.h"
#include "src/ir/datalog/input_relation_fact.h"
#include "src/ir/datalog/value.h"

namespace raksha::ir::datalog {

class OperandList : public Record<Symbol /*operand*/, OperandList /*next*/> {
  using Record::Record;
};

class ResultList : public Record<Symbol /*result*/, ResultList /*next*/> {
  using Record::Record;
};

using Operation =
    Record<Symbol /*owner*/, Symbol /*operator*/, ResultList /*result*/,
           OperandList /*operands*/, AttributeList /*attributes*/>;

class IsOperationFact : public InputRelationFact<Operation> {
 public:
  using InputRelationFact::InputRelationFact;
  IsOperationFact(IsOperationFact &&) = default;
  IsOperationFact &operator=(IsOperationFact &&) = default;
  virtual ~IsOperationFact() {}
  static constexpr std::string_view relation_name() { return "isOperation"; }
  std::string_view GetRelationName() const { return relation_name(); }
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_OPERATION_H_
