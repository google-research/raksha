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

#ifndef SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_ATTRIBUTE_H_
#define SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_ATTRIBUTE_H_

#include "src/backends/policy_enforcement/souffle/value.h"

namespace raksha::backends::policy_enforcement::souffle {

constexpr char kStringAttributePayloadName[] = "StringAttributePayload";
constexpr char kNumberAttributePayloadName[] = "NumberAttributePayload";

class AttributePayload : public Adt {
  using Adt::Adt;
};

class StringAttributePayload : public AttributePayload {
 public:
  StringAttributePayload(Symbol symbol)
      : AttributePayload(kStringAttributePayloadName) {
    members_.push_back(std::make_unique<Symbol>(symbol));
  }
};

class NumberAttributePayload : public AttributePayload {
 public:
  NumberAttributePayload(Number number)
      : AttributePayload(kNumberAttributePayloadName) {
    members_.push_back(std::make_unique<Number>(number));
  }
};

using Attribute =
    Record<Symbol /*attr_name*/, AttributePayload /*attr_payload*/>;

class AttributeList
    : public Record<Attribute /*attr*/, AttributeList /*next*/> {
 public:
  using Record::Record;
};

}  // namespace raksha::backends::policy_enforcement::souffle

#endif  // SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_ATTRIBUTE_H_
