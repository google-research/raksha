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

#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"
#include "src/common/utils/filesystem.h"
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
    absl::StatusOr<std::filesystem::path> temp_dir_statusor =
        common::utils::CreateTemporaryDirectory();
    CHECK(temp_dir_statusor.ok());
    directory_ = temp_dir_statusor.value();
  }

  // Usually, the test fixture's destructor is fine. But I'm pretty sure that
  // `remove_all` can throw an exception in general, which would cause undefined
  // behavior if we put this in the destructor. This probably isn't possible
  // with the noexception flags that we're using, but when it comes to undefined
  // behavior in dtors, better safe than sorry.
  void TearDown() override {
    if (!directory_.empty()) {
      std::filesystem::remove_all(directory_);
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
  std::filesystem::path directory_;
};

MATCHER(PathAsStringEq, "") {
  const std::filesystem::directory_entry& entry = testing::get<0>(arg);
  std::string path = testing::get<1>(arg);
  return entry.path().string() == path;
}

TEST_F(DumpFactsToDirectoryTest,
       DefaultInvocationDumpsIsOperationInTempDirectory) {
  absl::Status status = facts_.DumpFactsToDirectory(directory_);
  ASSERT_TRUE(status.ok());
  auto directory_iter =
      std::filesystem::directory_iterator(directory_.string());
  EXPECT_THAT(GetFilesInDirectory(directory_),
              testing::UnorderedElementsAre("isOperation.facts"));
  CheckFileContentsInDirectory(directory_);
}

TEST_F(DumpFactsToDirectoryTest, CreatesEmptyRelationsIfRequested) {
  std::vector<std::string> empty_relations = {"check", "claim"};
  absl::Status status =
      facts_.DumpFactsToDirectory(directory_, empty_relations);
  ASSERT_TRUE(status.ok());
  auto directory_iter =
      std::filesystem::directory_iterator(directory_.string());
  auto files = GetFilesInDirectory(directory_);
  EXPECT_THAT(files, testing::UnorderedElementsAre(
                         "isOperation.facts", "check.facts", "claim.facts"));
  CheckFileContentsInDirectory(directory_, empty_relations);
}

TEST_F(DumpFactsToDirectoryTest, CreatesFilesInRequestedDirectory) {
  std::vector<std::string> empty_relations = {"check", "claim"};

  char filename[] = "/tmp/foo.XXXXXX";
  ASSERT_NE(::mkdtemp(filename), nullptr);
  std::filesystem::path desired_path(filename);

  absl::Status status =
      facts_.DumpFactsToDirectory(desired_path, empty_relations);
  ASSERT_TRUE(status.ok());

  auto directory_iter =
      std::filesystem::directory_iterator(desired_path.string());
  auto files = GetFilesInDirectory(desired_path);
  EXPECT_THAT(files, testing::UnorderedElementsAre(
                         "isOperation.facts", "check.facts", "claim.facts"));
  CheckFileContentsInDirectory(desired_path, empty_relations);

  ASSERT_TRUE(std::filesystem::remove_all(desired_path));
}

TEST_F(DumpFactsToDirectoryTest, ReturnsErrorIfRequestedDirectoryDoesNotExist) {
  std::filesystem::path desired_path("nosuchpath");
  ASSERT_FALSE(std::filesystem::exists(desired_path));

  absl::Status status = facts_.DumpFactsToDirectory(desired_path, {});
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_EQ(
      status.message(),
      absl::StrFormat("Requested directory `%s` is not found!", desired_path));
}

TEST_F(DumpFactsToDirectoryTest,
       ReturnsErrorIfFactsFileAlreadyExistsInDirectory) {
  // Dump files to a temporary directory.
  absl::Status status = facts_.DumpFactsToDirectory(directory_);
  ASSERT_TRUE(status.ok());
  // Call again with the same directory to trigger an error.
  absl::Status second_status = facts_.DumpFactsToDirectory(directory_);
  EXPECT_FALSE(second_status.ok());
  EXPECT_EQ(second_status.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_EQ(second_status.message(),
            absl::StrFormat("Facts file `%s/isOperation.facts` already exists!",
                            directory_));
}

}  // namespace

}  // namespace raksha::backends::policy_engine::souffle
