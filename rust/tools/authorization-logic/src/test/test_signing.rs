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
    use crate::{ast::*, signing::tink_interface::*, utils::*};
  
    fn get_output_path(out_dir: &str, file: &str) -> String {
      format!("{}/{}", &utils::get_or_create_output_dir(out_dir), file)
    }
  
    #[test]
    fn test_sig_checking() {
        // Generate a keypair.
        store_new_keypair_cleartext(
            &get_output_path("test_keys", "andrew_pub.json"),
            &get_output_path("test_keys", "andrew_priv.json"),
        );

        let test_claim1 = vec![AstAssertion::AstFactAssertion {
            f: AstFact::AstFlatFactFact {
                f: AstFlatFact::AstPredFact {
                    p: AstPredicate {
                        sign: Sign::Positive,
                        name: "isInLondon".to_string(),
                        args: vec!["Highbury".to_string()],
                    },
                },
            },
        }];

        sign_claim(
            &get_output_path("test_keys", "andrew_priv.json"),
            &get_output_path("test_outputs", "claim1.sig"),
            &test_claim1,
        );

        // The signature check should pass.
        assert!(verify_claim(
            &get_output_path("test_keys", "andrew_pub.json"),
            &get_output_path("test_outputs", "claim1.sig"),
            &test_claim1
        )
        .is_ok());

        let test_claim2 = vec![AstAssertion::AstFactAssertion {
            f: AstFact::AstFlatFactFact {
                f: AstFlatFact::AstPredFact {
                    p: AstPredicate {
                        sign: Sign::Positive,
                        name: "isInLondon".to_string(),
                        args: vec!["Neptune".to_string()],
                    },
                },
            },
        }];

        // The signature check should fail.
        assert!(!verify_claim(
            &get_output_path("test_keys", "andrew_pub.json"),
            &get_output_path("test_outputs", "claim1.sig"),
            &test_claim2
        )
        .is_ok());
    }

    #[test]
    fn test_sig_checking_w_serialization() {
        store_new_keypair_cleartext(
            &get_output_path("test_keys", "andrew_pub2.json"),
            &get_output_path("test_keys", "andrew_priv2.json"),
        );

        let test_claim2 = vec![AstAssertion::AstFactAssertion {
            f: AstFact::AstFlatFactFact {
                f: AstFlatFact::AstPredFact {
                    p: AstPredicate {
                        sign: Sign::Positive,
                        name: "isInLondon".to_string(),
                        args: vec!["Highbury".to_string()],
                    },
                },
            },
        }];

        // This code sets up the files as though a claim
        // has been sent already so that the receiving side can be tested.
        sign_claim(
            &get_output_path("test_keys", "andrew_priv2.json"),
            &get_output_path("test_outputs", "claim2.sig"),
            &test_claim2,
        );

        serialize_to_file(
            &test_claim2,
            &get_output_path("test_outputs", "claim2.obj")).unwrap();

        // This code models the recipient side by deserializing the sent claims.
        let deser_claim = deserialize_from_file(
            &get_output_path("test_outputs", "claim2.obj")).unwrap();

        assert!(verify_claim(
            &get_output_path("test_keys", "andrew_pub2.json"),
            &get_output_path("test_outputs", "claim2.sig"),
            &deser_claim
        )
        .is_ok());
    }
}
