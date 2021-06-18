pub mod export_assertions;
pub mod import_assertions;

// the only reason tink is not encapsulated is that
// store_new_keypair_cleartext is used for testing
// purposes. It might be better to just write a separate
// script that generates keypairs.
pub mod tink_interface;
