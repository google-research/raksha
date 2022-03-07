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

//! The file `handle_imports` is a compiler pass that imports assertions made by
//! another principal by deserializing them from a file. Imported
//! assertions must be accompanied by an ECDSA signature (also serialized) and
//! using a keypair that is assocated with the principal
//! that made the assertion. The signature is checked during this pass.
//! If the signature checks pass, the imported assertions are added
//! to the AST.

use crate::ast::*;
use crate::signing::tink_interface::*;
use crate::utils::*;
use std::collections::HashMap;

type BindingEnv = HashMap<AstPrincipal, String>;

fn collect_pub_bindings(prog: &AstProgram) -> BindingEnv {
    prog.pub_binds
        .iter()
        .map(|kb| (kb.principal.clone(), kb.filename.clone()))
        .collect()
}

fn handle_one_import(kbenv: &BindingEnv, imp: &AstImport) -> AstSaysAssertion {
    let pubkey = utils::get_resolved_output_path(kbenv.get(&imp.principal).unwrap());
    let signature_file = utils::get_resolved_output_path(&(imp.filename.clone() + ".sig"));
    let full_import_path = utils::get_resolved_output_path(&(imp.filename.clone() + ".obj"));
    let assertion = deserialize_from_file(&full_import_path).unwrap();

    // TODO: A better error message is needed, potentially.
    verify_claim(&pubkey, &signature_file, &assertion).unwrap();

    AstSaysAssertion {
        prin: imp.principal.clone(),
        assertions: assertion,
        export_file: None,
    }
}

/// When given an AstProgram, this function handles its imports.
/// It is the only public interface to this compiler pass.
pub fn handle_imports(ast: &AstProgram) -> AstProgram {
    let bind_env = collect_pub_bindings(ast);
    let imported_assertions: Vec<_> = ast
        .imports
        .iter()
        .map(|s| handle_one_import(&bind_env, s))
        .collect();
    AstProgram {
        relation_declarations: ast.relation_declarations.clone(),
        assertions: [ast.assertions.clone(), imported_assertions].concat(),
        queries: ast.queries.clone(),
        imports: Vec::new(),
        priv_binds: ast.priv_binds.clone(),
        pub_binds: ast.pub_binds.clone(),
    }
}
