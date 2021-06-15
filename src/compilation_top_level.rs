use std::fs;
use crate::ast::*;
use crate::parsing::astconstructionvisitor;
use crate::souffle::souffle_interface;
use crate::signing::export_assertions;
use crate::signing::import_assertions;

// This has the top level function to compile to souffle. This is a separate file and module from 
// souffle-interface because part of the compilation step involces importing and exporting claims. 
// The importing and exporting of claims happens at the AST level rather than an IR and works 
// independently of the fact that the particular choice of solver is datalog. This separation might 
// make it easier to switch to a different solver later. This is also separate from main because
// "compile" is also used for tests

fn source_file_to_ast(filename: &String, in_dir: &String) -> AstProgram {
    let source = fs::read_to_string(&format!("{}/{}", in_dir, filename))
        .expect("failed to read input in input_to_souffle_file");
    astconstructionvisitor::parse_program(&source[..])
}

pub fn compile(filename: &String, in_dir: &String, out_dir: &String) {
    let prog = source_file_to_ast(filename, in_dir);
    let progWithImports = import_assertions::handle_imports(&prog);
    souffle_interface::ast_to_souffle_file(&progWithImports,
                                        filename, out_dir);
    export_assertions::export_assertions(&progWithImports);
}
