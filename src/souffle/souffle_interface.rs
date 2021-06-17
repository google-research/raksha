// This file contains code for interfacing with souffle (a dialect and 
// implementation of Datalog)
use std::fs;
use std::process::Command;

use crate::parsing::astconstructionvisitor;
use crate::souffle::lowering_ast_datalog::*;
use crate::souffle::souffle_emitter::*;
use crate::ast::*;

// Given a filename containing policy code: parses it, constructs
// an AST, translates the AST to the datalogIR, and emits a string
// contianing souffle code.
pub fn emit_souffle(filename: &String) {
    let source = fs::read_to_string(filename)
        .expect("failed to read input in emit_souffle");
    let prog = astconstructionvisitor::parse_program(&source[..]);
    let dlir_prog = LoweringToDatalogPass::lower(&prog);
    let souffle_code = SouffleEmitter::emit_program(&dlir_prog);
    println!("souffle code from {}, \n{}", &filename, souffle_code);
}

// Given a filename containing policy code: parses it, translates it
// to datalogIR, and emits souffle code to a new file with the same name 
// as the input but with the .dl extension in the out_dir
pub fn input_to_souffle_file(filename: &String, in_dir: &String,
                             out_dir: &String) {
    let source = fs::read_to_string(&format!("{}/{}", in_dir, filename))
        .expect("failed to read input in input_to_souffle_file");
    let prog = astconstructionvisitor::parse_program(&source[..]);
    let dlir_prog = LoweringToDatalogPass::lower(&prog);
    let souffle_code = SouffleEmitter::emit_program(&dlir_prog);
    fs::write(&format!("{}/{}.dl", out_dir, filename), souffle_code)
        .expect("failed to write output to file");
}

// Given a parsed AstProgram, this emits souffle code to a file. This is needed
// in addition to the above function since other passes that work on the highest
// level IR besides the one used to emit the main code.
pub fn ast_to_souffle_file(prog: &AstProgram, filename: &String,
                           out_dir: &String) {
    let dlir_prog = LoweringToDatalogPass::lower(&prog);
    let souffle_code = SouffleEmitter::emit_program(&dlir_prog);
    fs::write(&format!("{}/{}.dl", out_dir, filename), souffle_code)
        .expect("failed to write output to file");
}

// Calls the souffle command on a .dl file. CSVs generated by souffle
// are placed in outdir.
pub fn run_souffle(filename: &String, outdir: &String) {
    Command::new("souffle")
        .arg(&filename)
        .arg(&format!("-D{}/", outdir))
        .spawn()
        .expect("had an error in run_souffle()")
        .wait_with_output()
        .expect("had an error with souffle outputs");
}

// Returns true if the contents of a file are empty. This is used
// to check if queries are true. Queries are translated into souffle
// assertions that contain one entry if they are true and are empty
// if they are false. These queries are emitted as CSV files by souffle.
pub fn is_file_empty(filename: &String) -> bool {
    let contents = fs::read_to_string(filename)
        .expect("is_file_empty failed to read");
    contents == "" 
}
