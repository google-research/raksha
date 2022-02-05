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
#ifndef SRC_IR_DATA_DECL_H_
#define SRC_IR_DATA_DECL_H_

#include "absl/container/flat_hash_map.h"
#include "src/ir/types/type.h"

namespace raksha::ir {

// A data declaration, which corresponds to an input or output of a block.
class DataDecl {
 public:
  DataDecl(absl::string_view name, types::Type type)
      : name_(name), type_(std::move(type)) {}

  absl::string_view name() const { return name_; }
  const types::Type& type() const { return type_; }

 private:
  std::string name_;
  types::Type type_;
};

class DataDeclCollection {
 public:
  // Adds a declaration to the collection. Fails if there is already a
  // declaration with the given name.
  const DataDecl& AddDecl(absl::string_view name, types::Type type) {
    auto insertion_result = decls_.insert(
        {std::string(name), std::make_unique<DataDecl>(name, std::move(type))});
    CHECK(insertion_result.second)
        << "Adding a duplicate declaration for `" << name << "`.";
    return *(insertion_result.first)->second.get();
  }

  const DataDecl* FindDecl(absl::string_view name) {
    auto find_result = decls_.find(name);
    return (find_result == decls_.end()) ? nullptr : find_result->second.get();
  }

 private:
  absl::flat_hash_map<std::string, std::unique_ptr<DataDecl>> decls_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_DATA_DECL_H_
