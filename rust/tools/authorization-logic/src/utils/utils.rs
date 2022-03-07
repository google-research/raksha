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

/* Returns a path by appending bazel WORKSPACE root as needed. */
pub fn get_resolved_path(path: &str) -> String {
    if cfg!(test) && cfg!(feature = "bazel_build") {
        format!("rust/tools/authorization-logic/{}", path)
     } else {
        format!("{}", path)
     }
}

pub fn get_or_create_output_dir(path: &str) -> String {
    if cfg!(test) && cfg!(feature = "bazel_build") {
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
    if cfg!(test) && cfg!(feature = "bazel_build") {
        format!("{}/{}", std::env::temp_dir().display(), path)
    } else {
        (*path).to_string()
    }
}


// pub fn create_output_file(path: &str) -> Result<File, std::io::Error> {
//     File::create(&get_resolved_output_path(path))
// }

// pub fn open_input_file(path: &str) -> Result<File, std::io::Error> {
//     println!("Opening {} for input", get_resolved_path(path).to_string());
//     File::open(&get_resolved_path(path))
// }

