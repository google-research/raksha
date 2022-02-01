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
//-----------------------------------------------------------------------------
#ifndef SRC_XFORM_TO_DATALOG_AUTHORIZATION_LOGIC_H_
#define SRC_XFORM_TO_DATALOG_AUTHORIZATION_LOGIC_H_

#include <filesystem>

#include "absl/strings/string_view.h"

namespace raksha::xform_to_datalog {

int GenerateDatalogFactsFromAuthorizationLogic(
    absl::string_view program,
    const std::filesystem::path &program_dir,
    const std::filesystem::path &result_dir,
    const std::vector<absl::string_view> &relations_to_not_declare);

}

#endif  // SRC_XFORM_TO_DATALOG_AUTHORIZATION_LOGIC_H_

