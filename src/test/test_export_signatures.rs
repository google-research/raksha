#[cfg(test)]
mod test {
    use std::fs;
    use crate::ast::*;
    use crate::compilation_top_level::*;
    use crate::souffle::souffle_interface::*;

    // this is used for generating keypairs:
    use crate::signing::tink_interface::*;

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
