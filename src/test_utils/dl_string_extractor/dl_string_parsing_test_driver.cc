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
//
// This file is a simple driver to run datalog scripts generated from
// extracting raw strings representing parts of a datalog program to ensure
// that they parse correctly.
//
#include <memory>

#include "souffle/SouffleInterface.h"

// TODO(#160): Move this test driver to a common location with other test
//  drivers or roll all into a single test driver binary.
int main(int argc, char **argv) {
  // The only command line arg should be the name of the datalog module that
  // should be loaded.
  assert(argc == 2);
  std::string const test_name = std::string(argv[1]);

   std::unique_ptr<souffle::SouffleProgram> prog(
      souffle::ProgramFactory::newInstance(test_name));
  assert(prog != nullptr);

  // We want to make sure that the program can run, but aren't particularly
  // interested in its output. We're just checking that the extracted strings
  // compiled correctly into a runnable program.
  prog->run();

  return 0;
}
