// This is adapted from:
// https://github.com/rrevenantt/antlr4rust/blob/master/build.rs

use std::convert::TryInto;
use std::env;
use std::env::VarError;
use std::error::Error;
use std::fs::{read_dir, DirEntry, File};
use std::io::Write;
use std::path::Path;
use std::process::Command;

fn run_antlr_on(
    grammar_file: &str,
    antlr_path: &str
    )-> Result<(), Box<Error>> {

    let parsing_subdir = env::current_dir().unwrap().join("src/parsing");
    let full_file = grammar_file.to_owned() + ".g4";

    Command::new("java")
        .current_dir(parsing_subdir)
        .arg("-jar")
        .arg(antlr_path)
        .arg("-Dlanguage=Rust")
        .arg(&full_file)
        .arg("-visitor") // This is not actually strictly needed
        .spawn()
        .expect("antlr4rust jar failed to start")
        .wait_with_output()?;

    println!("cargo:rerun-if-changed=src/parsing/{}.g4", grammar_file);

    Ok(())
}

fn main() {

    // At the moment, this jar is included locally. See these instructions to
    // build it:
    // https://github.com/rrevenantt/antlr4rust

    let antlr_path = env::current_dir()
        .unwrap()
        .to_str()
        .unwrap()
        .to_owned()
        + "antlr4rust.jar";

    run_antlr_on("AuthLogic", &antlr_path);
    println!("hello build");

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=antlr4rust.jar");

    
}
