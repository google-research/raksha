// This is a compiler pass that imports assertions made by 
// another principal by deserializing them from a file. Imported
// assertions must be accompanied by an ECDSA signature (also serialized) and
// using a keypair that is assocated with the principal
// that made the assertion. The signature is checked during this pass.
// If the signature checks pass, the imported assertions are added
// to the AST.

use crate::ast::*;
use crate::signing::tink_interface::*;
use std::collections::HashMap;

type BindingEnv = HashMap<AstPrincipal, String>;

fn collect_pub_bindings(prog: &AstProgram) -> BindingEnv {
    prog.pub_binds.iter()
        .map(|kb| (kb.principal.clone(), kb.filename.clone()))
        .collect()
}

fn handle_one_import(kbenv: &BindingEnv, imp: &AstImport) -> AstSaysAssertion {
    let pubkey = kbenv.get(&imp.principal).unwrap();
    let signature_file = imp.filename.clone() + ".sig";
    let assertion = deserialize_from_file(
        &(imp.filename.clone() + ".obj")).unwrap();

    // TODO potentially a better error message is needed
    verify_claim(&pubkey, 
                 &signature_file,
                 &assertion)
        .unwrap();
    
    AstSaysAssertion { 
        prin: imp.principal.clone(),
        assertions: assertion,
        export_file: None
    }

}

pub fn handle_imports(ast: &AstProgram) -> AstProgram {
    let bindEnv = collect_pub_bindings(ast);
    let imported_assertions: Vec<_> = ast.imports.iter()
        .map(|s| handle_one_import(&bindEnv, s))
        .collect();

    AstProgram {
        assertions: [ast.assertions.clone(),
            imported_assertions].concat(),
        queries: ast.queries.clone(),
        imports: Vec::new(),
        priv_binds: ast.priv_binds.clone(),
        pub_binds: ast.pub_binds.clone()
    }
}
