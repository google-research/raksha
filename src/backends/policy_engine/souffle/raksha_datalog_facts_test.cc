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
//----------------------------------------------------------------------------
#include "src/backends/policy_engine/souffle/raksha_datalog_facts.h"

#include <filesystem>

#include "src/common/testing/gtest.h"
#include "src/common/utils/test/utils.h"
#include "src/ir/datalog/operation.h"
#include "src/ir/datalog/value.h"

namespace raksha::backends::policy_engine::souffle {
namespace {

using ir::datalog::Attribute;
using ir::datalog::AttributeList;
using ir::datalog::IsOperationFact;
using ir::datalog::OperandList;
using ir::datalog::Operation;
using ir::datalog::Symbol;

class DumpFactsToDirectoryTest : public testing::Test {
 public:
  DumpFactsToDirectoryTest() {
    // Initialize some IsOperationFacts for tests.
    IsOperationFact is_operation_facts[] = {
        IsOperationFact(Operation(
            Symbol("UserA"), Symbol("sql.Literal"), Symbol("out"),
            OperandList(),
            AttributeList(
                Attribute("literal_value", Attribute::String("number_5")),
                AttributeList()))),
        IsOperationFact(
            Operation(Symbol("UserB"), Symbol("sql.MergeOp"), Symbol("out"),
                      OperandList(Symbol("input1"),
                                  OperandList(Symbol("input2"), OperandList())),
                      AttributeList()))};
    for (auto&& fact : std::move(is_operation_facts)) {
      is_operation_file_facts_.push_back(fact.ToDatalogFactsFileString());
      facts_.AddIsOperationFact(std::move(fact));
    }
  }

  std::vector<std::string> GetFilesInDirectory(
      const std::filesystem::path& directory) {
    std::vector<std::string> result;
    for (const std::filesystem::directory_entry& f :
         std::filesystem::directory_iterator{directory}) {
      result.push_back(f.path().filename().string());
    }
    return result;
  }

  void CheckFileContentsInDirectory(
      const std::filesystem::path& directory,
      const std::vector<std::string>& empty_relations = {}) {
    EXPECT_THAT(utils::test::ReadFileLines(directory / "isOperation.facts"),
                testing::UnorderedElementsAreArray(is_operation_file_facts_));
    // Empty contents for empty_relations.
    for (const auto& relation : empty_relations) {
      EXPECT_THAT(utils::test::ReadFileLines(
                      directory / absl::StrFormat("%s.facts", relation)),
                  testing::UnorderedElementsAre());
    }
  }

 protected:
  RakshaDatalogFacts facts_;
  std::vector<std::string> is_operation_file_facts_;
};

MATCHER(PathAsStringEq, "") {
  const std::filesystem::directory_entry& entry = testing::get<0>(arg);
  std::string path = testing::get<1>(arg);
  return entry.path().string() == path;
}

TEST_F(DumpFactsToDirectoryTest,
       DefaultInvocationDumpsIsOperationInTempDirectory) {
  auto directory = facts_.DumpFactsToDirectory();
  ASSERT_TRUE(directory.ok());
  auto directory_iter =
      std::filesystem::directory_iterator(directory->string());
  EXPECT_THAT(GetFilesInDirectory(*directory),
              testing::UnorderedElementsAre("isOperation.facts"));
  CheckFileContentsInDirectory(*directory);
}

TEST_F(DumpFactsToDirectoryTest, CreatesEmptyRelationsIfRequested) {
  std::vector<std::string> empty_relations = {"check", "claim"};
  auto directory = facts_.DumpFactsToDirectory(empty_relations);
  ASSERT_TRUE(directory.ok());
  auto directory_iter =
      std::filesystem::directory_iterator(directory->string());
  auto files = GetFilesInDirectory(*directory);
  EXPECT_THAT(files, testing::UnorderedElementsAre(
                         "isOperation.facts", "check.facts", "claim.facts"));
  CheckFileContentsInDirectory(*directory, empty_relations);
}

TEST_F(DumpFactsToDirectoryTest, CreatesFilesInRequestedDirectory) {
  std::vector<std::string> empty_relations = {"check", "claim"};

  std::filesystem::path desired_path = std::tmpnam(nullptr);
  std::error_code error_code;
  bool tmp_dir_created =
      std::filesystem::create_directory(desired_path, error_code);
  ASSERT_TRUE(tmp_dir_created);

  auto directory = facts_.DumpFactsToDirectory(empty_relations, desired_path);
  ASSERT_TRUE(directory.ok());
  ASSERT_EQ(*directory, desired_path);

  auto directory_iter =
      std::filesystem::directory_iterator(directory->string());
  auto files = GetFilesInDirectory(*directory);
  EXPECT_THAT(files, testing::UnorderedElementsAre(
                         "isOperation.facts", "check.facts", "claim.facts"));
  CheckFileContentsInDirectory(*directory, empty_relations);
}

TEST_F(DumpFactsToDirectoryTest, ReturnsErrorIfRequestedDirectoryDoesNotExist) {
  std::filesystem::path desired_path = std::tmpnam(nullptr);
  auto directory = facts_.DumpFactsToDirectory({}, desired_path);
  EXPECT_FALSE(directory.ok());
  EXPECT_EQ(directory.status().code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_EQ(
      directory.status().message(),
      absl::StrFormat("Requested directory `%s` is not found!", desired_path));
}

TEST_F(DumpFactsToDirectoryTest,
       ReturnsErrorIfFactsFileAlreadyExistsInDirectory) {
  // Dump files to a temporary directory.
  auto directory = facts_.DumpFactsToDirectory();
  ASSERT_TRUE(directory.ok());
  // Call again with the same directory to trigger an error.
  auto new_directory = facts_.DumpFactsToDirectory({}, *directory);
  EXPECT_FALSE(new_directory.ok());
  EXPECT_EQ(new_directory.status().code(),
            absl::StatusCode::kFailedPrecondition);
  EXPECT_EQ(new_directory.status().message(),
            absl::StrFormat("Facts file `%s/isOperation.facts` already exists!",
                            *directory));
}

}  // namespace

}  // namespace raksha::backends::policy_engine::souffle
