#[cfg(test)]
mod test {
    use crate::signing::tink_interface::*;
    use crate::ast::*;
    // use tink_core::{keyset, TinkError};

    #[test]
    fn test_sig_checking() {
        // generate keypair
        store_new_keypair_cleartext(
            &"test_keys/andrew_pub.json".to_string(),
            &"test_keys/andrew_priv.json".to_string());

        let test_claim1 = vec![AstAssertion::AstFactAssertion {
            f: AstFact::AstFlatFactFact { f: AstFlatFact::AstPredFact {
                p: AstPredicate {
                    name: "isInLondon".to_string(),
                    args: vec!["Highbury".to_string()]
                }
            }}
        }];
        
        sign_claim(
            &"test_keys/andrew_priv.json".to_string(),
            &"test_outputs/claim1.sig".to_string(),
            &test_claim1
        );
        
        // signature check should pass
        assert!(verify_claim(
            &"test_keys/andrew_pub.json".to_string(),
            &"test_outputs/claim1.sig".to_string(),
            &test_claim1
        ).is_ok());

        let test_claim2 = vec![AstAssertion::AstFactAssertion {
            f: AstFact::AstFlatFactFact { f: AstFlatFact::AstPredFact {
                p: AstPredicate {
                    name: "isInLondon".to_string(),
                    args: vec!["Neptune".to_string()]
                }
            }}
        }];

        // signature check should fail
        assert!(!verify_claim(
            &"test_keys/andrew_pub.json".to_string(),
            &"test_outputs/claim1.sig".to_string(),
            &test_claim2
        ).is_ok());

    }

    #[test]
    fn test_sig_checking_w_serialization() {
        store_new_keypair_cleartext(
            &"test_keys/andrew_pub2.json".to_string(),
            &"test_keys/andrew_priv2.json".to_string());

        let test_claim2 = vec![AstAssertion::AstFactAssertion {
            f: AstFact::AstFlatFactFact { f: AstFlatFact::AstPredFact {
                p: AstPredicate {
                    name: "isInLondon".to_string(),
                    args: vec!["Highbury".to_string()]
                }
            }}
        }];
       
        // sender
        sign_claim(
            &"test_keys/andrew_priv2.json".to_string(),
            &"test_outputs/claim2.sig".to_string(),
            &test_claim2
        );

        serialize_to_file(
            &test_claim2,
            &"test_outputs/claim2.obj".to_string()
        ).unwrap();

        // receiver
        let deser_claim = deserialize_from_file(
            &"test_outputs/claim2.obj".to_string()
        ).unwrap();

        assert!(verify_claim(
            &"test_keys/andrew_pub2.json".to_string(),
            &"test_outputs/claim2.sig".to_string(),
            &deser_claim
        ).is_ok());

    }
}
