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

// Try blocks are used in antlr4-rust.
#![feature(try_blocks)]

mod ast;
mod compilation_top_level;
mod parsing;
mod signing;
mod souffle;
mod utils;
mod test;

use std::os::raw::c_char;
use std::os::raw::c_int;
use std::ffi::CStr;

#[no_mangle]
pub extern "C" fn generate_datalog_facts_from_authorization_logic(
    c_in_filename:  *const c_char,
    c_out_filename: *const c_char,
    c_decl_skip: *const c_char
) -> c_int {
  let result = std::panic::catch_unwind(|| {
    let input_filename = unsafe { CStr::from_ptr(c_in_filename) }.to_str().unwrap();
    let output_filename = unsafe { CStr::from_ptr(c_out_filename) }.to_str().unwrap();
    let decl_skip_vec = unsafe {
        CStr::from_ptr(c_decl_skip)
    }.to_str().unwrap().split(',').map(|s| s.to_string()).collect();
    // The CLI interface has been extended to take multiple files as inputs, but the C library does 
    // not make use of this because it is not needed, yet. Doing so would just entail changing this 
    // interface and the code that consumes it; the internals of the auth logic compiler should not 
    // need to change -- the following line can just call compile(...)
    // instead of compile_one_prgram(...)
    compilation_top_level::compile_one_program(input_filename,
                                   output_filename, &decl_skip_vec);
  });
  if result.is_err() {
    eprintln!("error: rust panicked");
    return 1;
  }
  return 0;
}
