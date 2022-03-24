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
//-----------------------------------------------------------------------------
//
// Policy checker for the multimic use case.
//
#include <iostream>
#include <memory>

#include "souffle/SouffleInterface.h"
#include "absl/strings/string_view.h"
#include "src/common/testing/gtest.h"
#include "src/common/logging/logging.h"

void AddEdge(const souffle::SouffleProgram *program, absl::string_view src,
             absl::string_view tgt) {
  souffle::Relation *rel = CHECK_NOTNULL(program->getRelation("edge"));
  souffle::tuple edge(rel); // Create an empty tuple
  edge << std::string(src) << std::string(tgt);
  rel->insert(edge);
  return;
}


TEST(CppInterfaceTest, CanLoadDatalogProgram) {
  std::unique_ptr<souffle::SouffleProgram> program(
      souffle::ProgramFactory::newInstance("raksha_policy_checker_cxx"));
  ASSERT_NE(program, nullptr);

  AddEdge(program.get(), "R.mic_audio_stream_out", "R.app_audio_stream_in");
  AddEdge(program.get(), "R.app_audio_stream_out", "R.asr_audio_stream_in");
  AddEdge(program.get(), "R.app_audio_stream_out", "R.store_audio_stream_in");

  program->run();
  souffle::Relation *disallowed_usage = program->getRelation("disallowedUsage");
  ASSERT_NE(disallowed_usage, nullptr);
  // .decl disallowedUsage(dataConsumer: Principal, usage: Usage, owner: Principal, tag: Tag)

  EXPECT_EQ(disallowed_usage->size(), 0);
  if (disallowed_usage->size() > 0) {
    std::string data_consumer, usage, owner, tag;
    LOG(WARNING) << "Disallowed Usages";
    for (auto &output : *disallowed_usage) {
      output >> data_consumer >> usage >> owner >> tag;
      LOG(WARNING) << "'" << usage << "' for  '" << tag
                   << "' of '" << owner << "'";
    }
  }
  // program->printAll();
}
