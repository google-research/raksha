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
#include <string_view>

#include "src/ir/datalog/value.h"

namespace raksha::ir::datalog {

using AttributePayload = Adt;

class Attribute
    : public Record<Symbol /*attr_name*/, AttributePayload /*attr_payload*/> {
 public:
  explicit Attribute(std::string_view name, AttributePayload payload)
      : Record(Symbol(name), std::move(payload)) {}

  class String : public AttributePayload {
   public:
    explicit String(Symbol symbol)
        : AttributePayload(kStringAttributePayloadName) {
      arguments_.push_back(std::make_unique<Symbol>(std::move(symbol)));
    }

    explicit String(std::string_view symbol) : String(Symbol(symbol)) {}
  };

  class Float : public AttributePayload {
   public:
    explicit Float(datalog::Float number)
        : AttributePayload(kFloatAttributePayloadName) {
      // NOTE: unique_ptr<datalog::Float> and **not** unique_ptr<Float>!
      arguments_.push_back(std::make_unique<datalog::Float>(std::move(number)));
    }

    explicit Float(double number) : Float(datalog::Float(number)) {}
  };

  class Number : public AttributePayload {
   public:
    template <typename T>
    Number(T value) = delete;

    Number(double value) = delete;
    Number(float value) = delete;
    Number(datalog::Float value) = delete;

    explicit Number(int number) : Number(datalog::Number(number)) {}
    explicit Number(datalog::Number number)
        : AttributePayload(kNumberAttributePayloadName) {
      // NOTE: unique_ptr<datalog::Number> and **not** unique_ptr<Number>!
      arguments_.push_back(
          std::make_unique<datalog::Number>(std::move(number)));
    }

    explicit Number(int64_t number) : Number(datalog::Number(number)) {}
  };

 private:
  static constexpr std::string_view kStringAttributePayloadName =
      "StringAttributePayload";
  static constexpr std::string_view kFloatAttributePayloadName =
      "FloatAttributePayload";
  static constexpr std::string_view kNumberAttributePayloadName =
      "NumberAttributePayload";
};

class AttributeList
    : public Record<Attribute /*attr*/, AttributeList /*next*/> {
 public:
  using Record::Record;
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_ATTRIBUTE_H_
