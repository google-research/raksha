/*
 * Copyright 2022 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Returns true if we are in the context of a `bazel test`.
pub fn is_bazel_test() -> bool {
  cfg!(test) && cfg!(feature = "bazel_build")
}

// If invoked within `bazel test` Returns a path by appending bazel TEST_TMPDIR.
// Otherwise, returns it unchanged.
pub fn get_resolved_path(path: &str) -> String {
    if is_bazel_test() {
        let tmp_dir = std::env::var("TEST_TMPDIR").expect("Couldn't read env var TEST_TMPDIR");
        let path = format!("{}/{}", tmp_dir, path);
        path
    } else {
        format!("{}", path)
    }
}

// Creates soft links to the data directories used in tests within the temporary workspace.
// This is a no-op if cfg!(not(feature = "bazel_build")).
#[cfg(test)]
pub fn setup_bazeltest_data_paths(input_paths: Vec<&str>) {
    if !is_bazel_test() { return }
    let tmp_dir = std::env::var("TEST_TMPDIR").expect("Couldn't read env var TEST_TMPDIR");
    let src_dir = std::env::var("TEST_SRCDIR").expect("Couldn't read env var TEST_SRCDIR");
    let workspace_dir = std::env::var("TEST_WORKSPACE")
        .expect("Couldn't read env var TEST_WORKSPACE");
    for input_path in input_paths {
        let resolved_input_path = format!(
            "{}/{}/rust/tools/authorization-logic/{}",
            src_dir, workspace_dir, input_path);
        let resolved_link_path = format!(
            "{}/{}", tmp_dir, input_path);
        std::process::Command::new("ln")
            .arg("-s")
            .arg(resolved_input_path)
            .arg(resolved_link_path)
            .output()
            .expect("had an error in output of `ln`");
    }
}

// Creates directories for test outputs within the temporary workspace.
// This is a no-op if cfg!(not(feature = "bazel_build")).
#[cfg(test)]
pub fn create_bazeltest_output_paths(output_paths: Vec<&str>) {
    if !is_bazel_test() { return }
    let tmp_dir = std::env::var("TEST_TMPDIR").expect("Couldn't read env var TEST_TMPDIR");
    for output_path in output_paths {
        let full_path = format!("{}/{}", tmp_dir, output_path);
        if !std::path::Path::new(&full_path).is_dir() {
            std::fs::create_dir_all(&full_path).expect("Creating directories failed");
        }
    }
}

// Setup the necessary directories for tests.
// This is a no-op if cfg!(not(feature = "bazel_build")).
#[cfg(test)]
pub fn setup_directories_for_bazeltest(input_paths: Vec<&str>, output_paths: Vec<&str>) {
    if !is_bazel_test() { return }
    setup_bazeltest_data_paths(input_paths);
    create_bazeltest_output_paths(output_paths);
}
