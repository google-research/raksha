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
// This file is a simple driver to run tests expressed as a datalog script that
// contains facts describing the dataflow graph (such as edges and tag claims)
// and our expectations (such as which tags will be present or absent). It
// returns 0 if the test has contents in the testPasses relation, 1 otherwise.
// This allows the datalog tests to be run from a bazel cc_test rule and avoids
// managing external facts files or diffing against an expected output file.
//
#include <iostream>
#include <memory>

#include "souffle/SouffleInterface.h"

int main(int argc, char **argv) {
  // We want one command line arg, the name of the current test module.
  assert(argc == 2);
  std::string const test_name = std::string(argv[1]);

  std::unique_ptr<souffle::SouffleProgram> prog(
      souffle::ProgramFactory::newInstance(test_name));
  assert(prog != nullptr);

  prog->run();

  souffle::Relation *test_passes =
    prog->getRelation("testPasses");

  int const test_passes_size = test_passes->size();
  assert((test_passes_size == 0) || (test_passes_size == 1));
  std::cout << "Test " << test_name;
  if (test_passes_size == 1) {
    std::cout << " passes." << std::endl;
    return 0;
  } else {
    std::cout << " fails." << std::endl;
    return 1;
  }
}
