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
mod test;

use std::env;
use structopt::StructOpt;

#[derive(StructOpt)]
#[structopt(name = "auth-logic", about = "An auth logic compiler.")]
struct Opt {
    /// The name of the auth logic program to compile
    // First positional argument (should be passed as a raw value, not --filename=...)
    #[structopt(required = true)]
    filename: String,

    /// Input directory where the auth logic program lives.
    /// If unspecified, it will be the current working directory.
    // Can be passed as -i or --in-dir
    #[structopt(short, long, env("PWD"))]
    in_dir: String,

    /// Output directory, where the compiled datalog program will be saved.
    /// If unspecified, it will be the current working directory.
    // Can be passed as -o or --out-dir
    #[structopt(short, long, env("PWD"))]
    out_dir: String,

    /// List of declarations to skip when generating Souffle code
    // Can be passed as -s or --skip. Passed as a comma-separated list.
    #[structopt(short = "s", long = "skip")]
    decl_skip: Vec<String>,

    /// Whether to skip running Souffle on the compiled datalog
    // Can be passed only as --skip-souffle
    #[structopt(long)]
    skip_souffle: bool,
}

fn main() {
    let opt = Opt::from_args();
    compilation_top_level::compile(&opt.filename, &opt.in_dir, &opt.out_dir, &opt.decl_skip);
    if (!opt.skip_souffle) {
        souffle::souffle_interface::run_souffle(
            &format!("{}/{}.dl", &opt.out_dir, &opt.filename),
            &opt.out_dir,
        );
    }
}
