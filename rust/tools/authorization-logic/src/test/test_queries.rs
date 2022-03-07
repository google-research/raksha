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
pub mod test {
    use crate::souffle::souffle_interface::*;
    use std::process::Command;
    use crate::utils::*;

    /// This struct gives the name of an authorization logic program in
    /// test_inputs, and a vector that relates the names of queries
    /// to the boolean values they should have.
    pub struct QueryTest<'a> {
        pub filename: &'a str,
        pub input_dir: &'a str,
        pub output_dir: &'a str,
        pub query_expects: Vec<(&'a str, bool)>,
        /* Query_expects is a vec not a HashMap because
         * there is no hashmap! macro for constructing
         * HashMap literals. There are crates providing
         * this macro, but limiting dependencies is
         * better for security/verification
        */
    }

    impl Default for QueryTest<'_> {
        fn default() -> Self {
            QueryTest {
                filename: "",
                input_dir: "test_inputs",
                output_dir: "test_outputs",
                query_expects: Vec::new(),
            }
        }
    }

    pub fn run_query_test(t: QueryTest) {
        let resolved_in_dir = utils::get_resolved_path(&t.input_dir.to_string());
        let resolved_out_dir = utils::get_or_create_output_dir(&t.output_dir.to_string());
        let resolved_test_outputs_dir = utils::get_or_create_output_dir("test_outputs");
        input_to_souffle_file(
            &t.filename.to_string(),
            &resolved_in_dir,
            &resolved_out_dir,
        );
        run_souffle(
            &format!("{}/{}.dl", resolved_out_dir, t.filename),
            &resolved_test_outputs_dir,
        );
        for (qname, intended_result) in t.query_expects {
            let queryfile = format!("{}/{}.csv", &resolved_test_outputs_dir, qname);
            assert!(is_file_empty(&queryfile) != intended_result);
        }
    }

    pub fn clean_test_dir() {
        Command::new("rm")
            .arg("-rf")
            .arg("test_outputs/*")
            .spawn()
            .expect("had an error when trying to start rm test_outputs/*")
            .wait_with_output()
            .expect("had an error in output of rm test_outputs/*");
    }

    #[test]
    fn test_conditions() {
        run_query_test(QueryTest {
            filename: "conditions",
            query_expects: vec![
                ("q_prin1_fact1", true),
                ("q_prin1_fact2", false),
                ("q_prin2_fact1", false),
            ],
            ..Default::default()
        });
    }

    #[test]
    fn test_delegations() {
        run_query_test(QueryTest {
            filename: "delegations",
            query_expects: vec![
                ("q_uncond1_t", true),
                ("q_uncond2_f", false),
                ("q_cond1_f", false),
                ("q_cond2_t", true),
                ("q_undel1_t", true),
                ("q_undel2_f", false),
            ],
            ..Default::default()
        });
    }

    #[test]
    fn test_canactas() {
        run_query_test(QueryTest {
            filename: "canActAs",
            query_expects: vec![("q_chicken", true), ("q_ketchup", false)],
            ..Default::default()
        });
    }
}
