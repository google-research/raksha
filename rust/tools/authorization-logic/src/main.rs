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

use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let filename = &args[1];
    let in_dir = &args[2];
    let out_dir = &args[3];
    let decl_skip = if(args.len() > 4) { &args[4] } else { "" };
    compilation_top_level::compile(filename, in_dir, out_dir, &decl_skip);
    souffle::souffle_interface::run_souffle(&format!("{}/{}.dl", out_dir,
                                                    filename), out_dir);
}
