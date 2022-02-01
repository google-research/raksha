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

#include <filesystem>
#include <fstream>
#include <iostream>

#include "src/common/logging/logging.h"
#include "src/test_utils/dl_string_extractor/datalog_string.h"

namespace extract = raksha::test_utils::dl_string_extractor;

// This extern declaration of GatherDatalogStrings allows us to link with the
// GatherDatalogStrings definition for a single test without resorting to
// even sketchier methods like conditional compilation or using regexes on
// the test file. We just link against the library containing the Datalog
// expected outputs and call its version of GatherDatalogStrings. If there
// are multiple definitions or no definitions, there will be a linker error.
namespace raksha::test_utils::dl_string_extractor {
extern std::vector<extract::DatalogString> GatherDatalogStrings();
}

// A simple test utility for extracting datalog strings and writing them out to
// a valid datalog file. We then can parse that datalog file to ensure that it
// parses, and thus that all of the strings that we generated are valid datalog.
int main(int argc, char **argv) {

  // Expect an output file as an argument.
  CHECK(argc == 2)
    << "dl_string_test_file_generator takes an output file as an argument.";

  // Set up the output file for writing.
  std::filesystem::path datalog_test_filepath(argv[1]);
  std::ofstream datalog_test_stream(
      datalog_test_filepath,
      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!datalog_test_stream) {
    LOG(FATAL) << "Error creating " << datalog_test_filepath << ":"
               << strerror(errno);
  }

  // Output headers we may need and declarations for relations.
  // Note: Most other escaped strings in the file have turned into raw
  // strings to improve readability. This one has been left because the
  // amount of newlines in it makes it actually more readable as an escaped
  // string.
  datalog_test_stream << "#include \"src/analysis/souffle/taint.dl\"\n\n"
                      << ".decl dummy(input_num: number)\n\n";

  // Get our datalog strings from the linked test and output the rules that
  // will check these snippets.
  std::vector datalog_strings =
      raksha::test_utils::dl_string_extractor::GatherDatalogStrings();
  uint64_t dummy_num = 0;
  for (const extract::DatalogString &dl_string : datalog_strings) {
    // Have only implemented rule bodies at this time.
    if (dl_string.kind() != extract::kDlRuleBody) { continue; }
    datalog_test_stream
      << "dummy(" << dummy_num++ << ") :- " << dl_string.dl_string() << ".\n";
  }
  return 0;
}
