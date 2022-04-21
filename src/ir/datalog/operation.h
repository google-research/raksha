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

#ifndef SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_OPERATION_H_
#define SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_OPERATION_H_

#include "src/backends/policy_enforcement/souffle/attribute.h"
#include "src/backends/policy_enforcement/souffle/fact.h"
#include "src/backends/policy_enforcement/souffle/value.h"

namespace raksha::backends::policy_enforcement::souffle {

class OperandList : public Record<Symbol /*operand*/, OperandList /*next*/> {
  using Record::Record;
};

using Operation =
    Record<Symbol /*owner*/, Symbol /*operator*/, Symbol /*result*/,
           OperandList /*operands*/, AttributeList /*attributes*/>;

constexpr char kIsOperationRelationName[] = "isOperation";

using IsOperationFact = Fact<kIsOperationRelationName, Operation>;

}  // namespace raksha::backends::policy_enforcement::souffle

#endif  // SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_OPERATION_H_
