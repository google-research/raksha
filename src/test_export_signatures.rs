#[cfg(test)]
mod test {
    use std::fs;
    use crate::signing::*;
    use crate::ast::*;
    use crate::souffle_interface;
    use crate::export_signatures;
    use crate::parsing::astconstructionvisitor;
    use tink_core::{keyset, TinkError};

    // These two functions are just copied from main. A refactor is needed.
    fn source_file_to_ast(filename: &String, in_dir: &String) -> AstProgram {
        let source = fs::read_to_string(&format!("{}/{}", in_dir, filename))
            .expect("failed to read input in input_to_souffle_file");
        astconstructionvisitor::parse_program(&source[..])
    }

    fn compile(filename: &String, in_dir: &String, out_dir: &String) {
        let prog = source_file_to_ast(filename, in_dir);
        souffle_interface::ast_to_souffle_file(&prog, filename, out_dir);
        export_signatures::export_assertions(&prog);
    }

    #[test]
    fn test_signature_exporting() {
        store_new_keypair_cleartext(
            &"test_keys/principal1_pub.json".to_string(),
            &"test_keys/principal1_priv.json".to_string());

        compile(&"exporting".to_string(), &"test_inputs".to_string(),
            &"test_outputs".to_string());

        let deser_claim = deserialize_from_file(
            &"test_outputs/prin1_statement1.obj".to_string()
        ).unwrap();

        assert!(verify_claim(
            &"test_keys/principal1_pub.json".to_string(),
            &"test_outputs/prin1_statement1.sig".to_string(),
            &deser_claim
        ).is_ok());

    }

}
