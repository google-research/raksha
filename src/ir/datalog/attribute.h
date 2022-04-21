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

#ifndef SRC_IR_DATALOG_ATTRIBUTE_H_
#define SRC_IR_DATALOG_ATTRIBUTE_H_

#include <memory>

#include "src/ir/datalog/value.h"

namespace raksha::ir::datalog {

constexpr char kStringAttributePayloadName[] = "StringAttributePayload";
constexpr char kNumberAttributePayloadName[] = "NumberAttributePayload";

class AttributePayload : public Adt {
 public:
  using Adt::Adt;
};

class StringAttributePayload : public AttributePayload {
 public:
  StringAttributePayload(Symbol symbol)
      : AttributePayload(kStringAttributePayloadName) {
    arguments_.push_back(std::make_unique<Symbol>(symbol));
  }
};

class NumberAttributePayload : public AttributePayload {
 public:
  NumberAttributePayload(Number number)
      : AttributePayload(kNumberAttributePayloadName) {
    arguments_.push_back(std::make_unique<Number>(number));
  }
};

using Attribute =
    Record<Symbol /*attr_name*/, AttributePayload /*attr_payload*/>;

class AttributeList
    : public Record<Attribute /*attr*/, AttributeList /*next*/> {
 public:
  using Record::Record;
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_ATTRIBUTE_H_
