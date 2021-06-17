use crate::ast::*;
use crate::signing::tink_interface::*;
use std::collections::HashMap;


// This compiler pass handles exporting of policy statements.
// For each SaysAssertion with an export clause, it serializes the object
// that corresponds to the (set of) assertions to a file and it produces an 
// ECDSA signature of that object in a separate file. It operates on the
// highest-level IR in the policy language (in ast.rs).

// Note that AstPrincipal implements Hash and Eq, so different
// occurances of the same principal within a program will not
// create multiple entries in this HashMap
type BindingEnv = HashMap<AstPrincipal, String>;

fn collect_priv_bindings(prog: &AstProgram) -> BindingEnv {
    prog.priv_binds.iter()
        .map(|kb| (kb.principal.clone(), kb.filename.clone()))
        .collect()
}

fn export_says_assertion(says_assertion: &AstSaysAssertion,
         priv_env: &BindingEnv) {
    match &says_assertion.export_file {
        None => { }
        Some(filename) => {
            let obj_file = filename.clone() + ".obj";
            let sig_file = filename.clone() + ".sig";
            let priv_key_file = priv_env.get(&says_assertion.prin).unwrap();
            serialize_to_file(&says_assertion.assertions, &obj_file).unwrap();
            sign_claim(&priv_key_file, &sig_file, &says_assertion.assertions);
        }
    }
}

pub fn export_assertions(prog: &AstProgram) {
    let priv_env = collect_priv_bindings(&prog);
    for assertion in &prog.assertions {
        export_says_assertion(&assertion, &priv_env);
    }
}
