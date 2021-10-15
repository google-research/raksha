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

#[cfg(test)]
mod test {
    use crate::{ast::*, compilation_top_level::*, souffle::souffle_interface::*};
    use std::fs;

    // This dependency is used for generating keypairs.
    use crate::signing::tink_interface::*;

    #[test]
    fn test_signature_exporting() {
        store_new_keypair_cleartext(
            "test_keys/principal1_pub.json",
            "test_keys/principal1_priv.json",
        );

        compile("exporting", "test_inputs", "test_outputs", &Vec::new());

        let deser_claim =
            deserialize_from_file(&"test_outputs/prin1_statement1.obj".to_string()).unwrap();

        assert!(verify_claim(
            &"test_keys/principal1_pub.json".to_string(),
            &"test_outputs/prin1_statement1.sig".to_string(),
            &deser_claim
        )
        .is_ok());
    }
}
