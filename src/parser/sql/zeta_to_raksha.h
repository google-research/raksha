//-------------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------

#include <iostream>
#include <sstream>

#include "absl/strings/string_view.h"
#include "zetasql/public/analyzer.h"
#include "zetasql/public/parse_helpers.h"
#include "zetasql/public/type.h"

namespace raksha::parser::sql {

std::string PrintParseResult(absl::string_view sql_text) {
  std::stringstream ss;
  std::unique_ptr<const zetasql::AnalyzerOutput> output;
  zetasql::AnalyzerOptions default_options;  // language options for changing
                                             // behavior of analysis/output
  // Catalog has to do with global namespaces. It seems like this
  // should be populated by parsing the input somehow?
  zetasql::Catalog* catalog = nullptr;
  // TypeFactory creates and owns type objects. I am not sure why
  // this is not encapsulated and is a part of the public
  // APIs... hmmm...
  zetasql::TypeFactory type_factory;
  absl::Status result = AnalyzeStatement(sql_text, default_options, catalog,
                                         &type_factory, &output);

  if (result != absl::OkStatus()) {
    ss << "AnalyzeStatement result failed: " << result << std::endl;
  } else {
    ss << output->resolved_statement()->DebugString();
  }
  return ss.str();
}

}  // namespace raksha::parser::sql