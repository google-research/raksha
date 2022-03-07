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
    use crate::{
        ast::*, compilation_top_level::*, souffle::souffle_interface::*,
        test::test_queries::test::QueryTest, utils::*,
    };
    use std::fs;

    // This dependency is used for generating keypairs.
    use crate::signing::tink_interface::*;

    fn get_output_path(out_dir: &str, file: &str) -> String {
      format!("{}/{}", &utils::get_or_create_output_dir(out_dir), file)
    }

    fn query_test_with_imports(t: QueryTest) {
        compile(t.filename, t.input_dir, t.output_dir, "");
        run_souffle(
            &utils::get_resolved_output_path(&format!("test_outputs/{}.dl", t.filename)),
            &utils::get_resolved_output_path("test_outputs")
        );
        for (qname, intended_result) in t.query_expects {
            let queryfile = utils::get_resolved_output_path(&format!("test_outputs/{}.csv", qname));
            assert!(is_file_empty(&queryfile) != intended_result);
        }
    }

    #[test]
    fn test_signature_importing() {
        store_new_keypair_cleartext(
            &get_output_path("test_keys", "principal1e_pub.json"),
            &get_output_path("test_keys", "principal1e_priv.json"),
        );

        // This code generates exported statements from test_inputs/exporting.
        compile("importing_export_half", "test_inputs", "test_outputs", "");

        // This code imports statements into test_inputs/importing
        // and checks queries for expected results.
        query_test_with_imports(QueryTest {
            filename: "importing",
            query_expects: vec![("q1", true), ("q2", false), ("q3", true), ("q4", false)],
            ..Default::default()
        })
    }
}
