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
// A simple driver of the datalog script for finding failing tag checks. This
// will just read all of the facts in a directory, run that script, and fail if
// any non-trivial output is generated.
//
#include <iostream>
#include <memory>

#include "souffle/SouffleInterface.h";

int main(int argc, char **argv) {
  assert(argc == 2);
  std::unique_ptr<souffle::SouffleProgram> prog(
      souffle::ProgramFactory::newInstance("find_failing_tag_checks"));
  assert(prog != nullptr);
  prog->loadAll(argv[1]);
  prog->run();
  souffle::Relation *failingCheckRel = prog->getRelation("failingTagCheck");
  assert(failingCheckRel != nullptr);
  if (failingCheckRel->size() > 0) {
    std::cerr << "Found failing tag checks:" << std::endl;
    prog->printAll();
  } else {
    return 0;
  }
}

