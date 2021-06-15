#[cfg(test)]
mod test {
    use std::fs;
    use crate::signing::*;
    use crate::ast::*;
    use crate::souffle_interface;
    use crate::export_signatures;
    use crate::parsing::astconstructionvisitor;
    use crate::import_assertions;
    use crate::testing_queries;
    use tink_core::{keyset, TinkError};
    use crate::testing_queries::test::QueryTest;
    use crate::souffle_interface::*;

    // These two functions are just copied from main. A refactor is needed.
    fn source_file_to_ast(filename: &String, in_dir: &String) -> AstProgram {
        let source = fs::read_to_string(&format!("{}/{}", in_dir, filename))
            .expect("failed to read input in input_to_souffle_file");
        astconstructionvisitor::parse_program(&source[..])
    }

    fn compile(filename: &String, in_dir: &String, out_dir: &String) {
        let prog = source_file_to_ast(filename, in_dir);
        let progWithImports = import_assertions::handle_imports(&prog);
        souffle_interface::ast_to_souffle_file(&progWithImports, filename, out_dir);
        export_signatures::export_assertions(&progWithImports);
    }

    fn query_test_with_imports(t: QueryTest) {
        compile(&t.filename.to_string(), 
                &"test_inputs".to_string(),
                &"test_outputs".to_string());
        run_souffle(&format!("test_outputs/{}.dl", t.filename),
                    &"test_outputs".to_string());
        for (qname, intended_result) in t.query_expects {
            let queryfile = format!("test_outputs/{}.csv", qname);
            assert!(is_file_empty(&queryfile) != intended_result);
        }
    }


    #[test]
    fn test_signature_importing() {
        store_new_keypair_cleartext(
            &"test_keys/principal1e_pub.json".to_string(),
            &"test_keys/principal1e_priv.json".to_string());

        // generate exported statements from test_inputs/exporting
        compile(&"importing_export_half".to_string(),
            &"test_inputs".to_string(),
            &"test_outputs".to_string());

        // import statements into test_inputs/importing
        // and check queries for expected results
        query_test_with_imports(QueryTest {
            filename: "importing",
            query_expects: vec!(
                ("q1", true),
                ("q2", false),
                ("q3", true),
                ("q4", false)
            )
        })
    }
}

