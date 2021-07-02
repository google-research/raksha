/*
 * Copyright 2021 The Raksha Authors
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

// This is adapted from:
// https://github.com/rrevenantt/antlr4rust/blob/master/build.rs

use std::{env, error::Error, process::Command};

fn run_antlr_on(grammar_file: &str, antlr_path: &str) -> Result<(), Box<dyn Error>> {
    let parsing_subdir = env::current_dir().unwrap().join("src/parsing");
    let full_file = grammar_file.to_owned() + ".g4";

    // When debugging either this build script or the antlr, code it may be
    // useful to build with
    // 'cargo build -vv &> output file'
    // because cargo omits errors generated from executed commands otherwise

    Command::new("java")
        .current_dir(parsing_subdir)
        .arg("-jar")
        .arg(antlr_path)
        .arg("-Dlanguage=Rust")
        .arg("-o")
        .arg("antlr_gen")
        .arg(&full_file)
        .spawn()
        .expect("antlr4rust jar failed to start")
        .wait_with_output()?;

    Ok(())
}

fn main() {
    // At the moment, the jar for antlr is included locally. See these
    // instructions to build it: https://github.com/rrevenantt/antlr4rust

    let antlr_path = format!(
        "{}/third_party/antlr4-rust/antlr4rust.jar",
        env::current_dir()
            .unwrap()
            .into_os_string()
            .into_string()
            .unwrap()
    );

    run_antlr_on("AuthLogic", &antlr_path).unwrap();
}
