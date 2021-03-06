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

use structopt::StructOpt;

#[derive(StructOpt)]
#[structopt(name = "auth-logic", about = "An authorization logic compiler.")]
struct Opt {
    /// The path of the input auth logic program to compile
    // First positional argument (should be passed as a raw value, not --filename=...)
    #[structopt(required = true)]
    input_filename: String,

    /// Output file path where compiled souffle program will be saved.
    // Can be passed as -o or --souffle_output_file
    #[structopt(short = "o", long)]
    souffle_output_file: String,

    /// Output directory, where the CSVs containing the results of evaluating
    /// queries will be saved.
    // Can be passed as -o or --out-dir
    #[structopt(short="q", long="output_queries_directory", env("PWD"))]
    output_queries_directory: String,

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
    compilation_top_level::compile(&opt.input_filename,
                                   &opt.souffle_output_file,
                                   &opt.decl_skip);
    if !opt.skip_souffle {
        souffle::souffle_interface::run_souffle(
            &opt.souffle_output_file,
            &opt.output_queries_directory,
        );
    };
}
