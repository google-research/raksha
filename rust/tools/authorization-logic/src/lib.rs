#![feature(try_blocks)]

mod ast;
mod compilation_top_level;
mod parsing;
mod signing;
mod souffle;

use std::os::raw::c_char;
use std::os::raw::c_int;
use std::ffi::CStr;

#[no_mangle]
pub extern "C" fn generate_datalog_facts_from_authorization_logic(
    c_filename:  *const c_char,
    c_in_dir: *const c_char,
    c_out_dir: *const c_char
) -> c_int {
  let result = std::panic::catch_unwind(|| {
    let filename = unsafe {
      CStr::from_ptr(c_filename).to_str().unwrap()
     };
     let in_dir = unsafe {
       CStr::from_ptr(c_in_dir).to_str().unwrap()
     };
     let out_dir = unsafe {
       CStr::from_ptr(c_out_dir).to_str().unwrap()
     };
     compilation_top_level::compile(filename, in_dir, out_dir);
  });
  if result.is_err() {
    eprintln!("error: rust panicked");
    return 1;
  }
  return 0;
}
