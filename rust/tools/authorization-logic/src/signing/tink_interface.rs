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
use crate::ast::*;
use std::{
    fs::File,
    io::{Error, Read, Write},
};
use tink_core::{keyset, TinkError};

/// The file `tink_interface.rs` contains all functions that interface between
/// this authorization logic implementation and the tink library for
/// cryptography. This is meant to help encapsulate the choice of particular
/// cryptographic library and the choices for cryptographic primitives and
/// serialization strategies used.

// The specific choice of how serialization is done is encapsulated because it
// is likely to change. Binaries are brittle way of serializing objects, but
// this was reasonable to get running and is easy to change later.
fn serialize_claim(claim: &Vec<AstAssertion>) -> Vec<u8> {
    bincode::serialize(&claim).unwrap()
}

fn deserialize_claim(claim_bin: Vec<u8>) -> Vec<AstAssertion> {
    bincode::deserialize(&claim_bin[..]).unwrap()
}

pub fn serialize_to_file(claim: &Vec<AstAssertion>, filename: &str) -> Result<(), Error> {
    let mut file = File::create(filename)?;
    file.write_all(&serialize_claim(claim))?;
    Ok(())
}

pub fn deserialize_from_file(filename: &str) -> Result<Vec<AstAssertion>, Error> {
    let mut file = File::open(filename)?;
    let mut contents = Vec::new();
    file.read_to_end(&mut contents)?;
    Ok(deserialize_claim(contents))
}

// At present, this function is only used in tests, so a warning will be given
// that this is not used. In the future, a script for generating keys might
// also use this outside of tests.
pub fn store_new_keypair_cleartext(pub_key_file: &str, priv_key_file: &str) {
    tink_signature::init();

    // Create a new signing keypair.
    let key_handle =
        tink_core::keyset::Handle::new(&tink_signature::ecdsa_p256_key_template()).unwrap();

    // Write the new private key.
    let prv_file = File::create(priv_key_file).unwrap();
    // See also tink_core::keyset::BinWriter/BinReader
    let mut prv_writer = keyset::JsonWriter::new(prv_file);
    // This stores the private key in plaintext, If possible, it would be better
    // to store the private key using a google cloud or aws key manager.
    keyset::insecure::write(&key_handle, &mut prv_writer).unwrap();

    // Write the new public key.
    let pub_key_handle = key_handle.public().unwrap();
    let pub_file = File::create(pub_key_file).unwrap();
    let mut pub_writer = keyset::JsonWriter::new(pub_file);
    keyset::insecure::write(&pub_key_handle, &mut pub_writer).unwrap();
}

/// The funciton `sign_claim` takes the name of a file containing a private ECDSA
/// key, the name of an output file that will store a signature, and a vector
/// of assertions. It serializes the vector of assertions, generates a signature
/// of this serialization using the given key, and writes this signature to a file.
pub fn sign_claim(priv_key_file: &str, signature_file: &str, claim: &Vec<AstAssertion>) {
    // Read private key from file.
    let prv_file = File::open(priv_key_file).unwrap();
    let mut json_reader = keyset::JsonReader::new(prv_file);
    let priv_kh = keyset::insecure::read(&mut json_reader).unwrap();

    // Write the signature to a file.
    let bytes = serialize_claim(&claim);
    let signer = tink_signature::new_signer(&priv_kh).unwrap();
    let signature = signer.sign(&bytes).unwrap();

    let mut sig_file = File::create(signature_file).unwrap();
    sig_file.write(&signature).unwrap();
}

/// The function `verify_claim` takes the name of a file contianing a public
/// ECDSA key, the name of a file storing a signature for a vector of
/// AstAssertions, and a vector of AstAssertions. It uses the given public key
/// to check the signature against the object and throws an error if the check
/// fails.
pub fn verify_claim(
    pub_key_file: &str,
    signature_file: &str,
    claim: &Vec<AstAssertion>,
) -> Result<(), TinkError> {
    // Read the pub key from a file.
    let pub_file = File::open(pub_key_file).unwrap();
    let mut json_reader = keyset::JsonReader::new(pub_file);
    let pub_kh = keyset::insecure::read(&mut json_reader).unwrap();

    let plaintext = serialize_claim(&claim);
    let v = tink_signature::new_verifier(&pub_kh).unwrap();
    let mut signature_buf = Vec::new();
    File::open(&signature_file)
        .unwrap()
        .read_to_end(&mut signature_buf)
        .unwrap();
    v.verify(&signature_buf, &plaintext)
}
