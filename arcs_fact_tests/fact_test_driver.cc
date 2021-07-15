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
// Just a simple example to check that we can use the Souffle C++ interface.
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

  souffle::Relation *bad_expectations =
    prog->getRelation("badTagCheckExpectation");
  souffle::Relation *unfulfilled_expectations =
    prog->getRelation("tagCheckNotAsExpected");
  assert(bad_expectations != nullptr);
  assert(unfulfilled_expectations != nullptr);

  char const *test_status_explanation = nullptr;
  bool success = false;
  if (bad_expectations->size() > 0) {
    test_status_explanation = "has malformed expectations.";
  } else if (unfulfilled_expectations->size() > 0) {
    test_status_explanation = "has results that differ from expectations.";
  } else {
    test_status_explanation = "matched all expectations.";
    success = true;
  }

  std::cout << "Test " << test_name << " " << test_status_explanation << std::endl;
  if (success) {
    return 0;
  } else {
    prog->printAll();
    return 1;
  }
}
