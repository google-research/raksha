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

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "src/common/logging/logging.h"
#include "src/ir/auth_logic/ast_construction.h"
#include "src/ir/datalog/program.h"
#include "src/ir/auth_logic/ast.h"

ABSL_FLAG(std::string, file_name, "", "The file with authorization logic facts");
ABSL_FLAG(std::string, auth_input_dir, "", "The  input directory.");
ABSL_FLAG(std::string, auth_output_dir, "", "output directory for the datalog");

int main(int argc, char *argv[]) {

    absl::ParseCommandLine(argc, argv);
    //verify command line arguements
    std::filesystem::path auth_input_filepath(absl::GetFlag(FLAGS_auth_input_dir));
    if (!std::filesystem::exists(auth_input_dir)) {
        LOG(ERROR) << "Auth input file directory " << auth_input_dir
                << " does not exist!";
        return 1;
    }
    std::filesystem::path auth_output_filepath(absl::GetFlag(FLAGS_auth_output_dir));
    if (!std::filesystem::exists(auth_output_dir)) {
        LOG(ERROR) << "Auth output file directory " << auth_input_dir
                << " does not exist!";
        return 1;
    }
    // std::filesystem::path auth_file_name(absl::GetFlag(FLAGS_file_name));
    // if (!std::filesystem::exists(auth_file_name)) {
    //     LOG(ERROR) << "Auth output file directory " << auth_file_name
    //             << " does not exist!";
    //     return 1;
    // }

    //compile
        //source file to AST
        std::filesystem::path auth_input_filename = auth_input_filepath.filename();
        //Read file into a string
        std::ifstream file_stream(auth_input_filename);
        if (!file_stream) {
            LOG(ERROR) << "Unable to read contents in authorization logic input file\n";
            return std::nullopt;
        }
        //file_stream = "Harsha";
        Program prog = parse_program(&file_stream);
        /
        //insert assertions  -> To-do
        //ast to souffle file
        
        //export assertions
       
    //run souffle(souffle interface)
     /// Given a parsed AstProgram, this function emits souffle code to a file.

