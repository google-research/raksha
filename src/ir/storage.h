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
#ifndef SRC_IR_STORAGE_H_
#define SRC_IR_STORAGE_H_

#include "src/ir/access_path_v2.h"
#include "src/ir/data_decl.h"

namespace raksha::ir {

class Storage {
 public:
  Storage(DataDecl decl): decl_(std::move(decl)) {}

  // TODO: Delete copy constructor.

  void AddWriteSource(v2::AccessPath access_path) {
    write_sources_.push_back(access_path);
  }

  void AddReadTarget(v2::AccessPath access_path) {
    read_targets_.push_back(access_path);
  }

 private:
  DataDecl decl_;
  std::vector<v2::AccessPath> read_targets_;
  std::vector<v2::AccessPath> write_sources_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_STORAGE_H_
