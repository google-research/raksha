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

use std::fs::File;

// Returns true if we are in the context of a `bazel test`.
pub fn is_bazel_test() -> bool {
  cfg!(test) && cfg!(feature = "bazel_build")
}

/* Returns a path by appending bazel WORKSPACE root as needed. */
pub fn get_resolved_path(path: &str) -> String {
    if is_bazel_test() {
        format!("rust/tools/authorization-logic/{}", path)
     } else {
        format!("{}", path)
     }
}

pub fn get_or_create_output_dir(path: &str) -> String {
    if is_bazel_test() {
        let full_path = format!("{}/{}", std::env::temp_dir().display(), path);
        if !std::path::Path::new(&full_path).is_dir() {
            println!("Full path is {}", full_path);
            std::fs::create_dir_all(&full_path);
        }
        full_path
    } else {
        (*path).to_string()
    }
}

pub fn get_resolved_output_path(path: &str) -> String {
    if is_bazel_test() {
        format!("{}/{}", std::env::temp_dir().display(), path)
    } else {
        (*path).to_string()
    }
}

// Creates soft links to the data directories used in tests within the temporary workspace.
#[cfg(feature = "bazel_build")]
#[cfg(test)]
pub fn setup_test_data_paths(input_paths: Vec<&str>) {
    assert!(is_bazel_test(), "Not supported outside bazel tests.");
    let tmp_dir = std::env::var("TEST_TMPDIR").unwrap();
    for input_path in input_paths {
        let resolved_input_path = format!(
            "{}/{}/rust/tools/authorization-logic/{}",
            tmp_dir,
            std::env::var("TEST_WORKSPACE").unwrap(),
            input_path);
        let resolved_link_path = format!(
            "{}/{}",
            tmp_dir,
            input_path);
        std::process::Command::new("ln")
            .arg("-s")
            .arg(resolved_input_path)
            .arg(resolved_link_path)
            .spawn()
            .expect("had an error when trying to start ls")
            .wait_with_output()
            .expect("had an error in output of ls");
    }
}

// Creates directories for test outputs within the temporary workspace.
// This is a no-op if cfg!(not(feature = "bazel_build")).
#[cfg(test)]
pub fn create_bazeltest_output_paths(output_paths: Vec<&str>) {
    if !is_bazel_test() { return }
    let tmp_dir = std::env::var("TEST_TMPDIR").unwrap();
    for output_path in output_paths {
        let full_path = format!("{}/{}", tmp_dir, output_path);
        if !std::path::Path::new(&full_path).is_dir() {
            println!("Creating output path is {}", full_path);
            std::fs::create_dir_all(&full_path);
        }
    }
}

// Setup the necessary direcotires for tests.
// This is a no-op if cfg!(not(feature = "bazel_build")).
#[cfg(test)]
pub fn setup_directories_for_bazeltest(input_paths: Vec<&str>, output_paths: Vec<&str>) {
    if !is_bazel_test() { return }
    setup_bazeltest_data_paths(input_paths);
    create_bazeltest_output_paths(output_paths);
}

// pub fn create_output_file(path: &str) -> Result<File, std::io::Error> {
//     File::create(&get_resolved_output_path(path))
// }

// pub fn open_input_file(path: &str) -> Result<File, std::io::Error> {
//     println!("Opening {} for input", get_resolved_path(path).to_string());
//     File::open(&get_resolved_path(path))
// }

