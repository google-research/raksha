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
        "{}/antlr4rust.jar",
        env::current_dir()
            .unwrap()
            .into_os_string()
            .into_string()
            .unwrap()
    );

    run_antlr_on("AuthLogic", &antlr_path).unwrap();
}
