load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

#--------------------
# googletest
#--------------------
http_archive(
    name = "com_google_googletest",
    sha256 = "5cf189eb6847b4f8fc603a3ffff3b0771c08eec7dd4bd961bfd45477dd13eb73",
    strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
    urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
)

# Protobuf:
#
# (See https://github.com/rules-proto-grpc/rules_proto_grpc)
http_archive(
    name = "rules_proto_grpc",
    sha256 = "507e38c8d95c7efa4f3b1c0595a8e8f139c885cb41a76cab7e20e4e67ae87731",
    strip_prefix = "rules_proto_grpc-4.1.1",
    urls = ["https://github.com/rules-proto-grpc/rules_proto_grpc/archive/4.1.1.tar.gz"],
)

load("@rules_proto_grpc//:repositories.bzl", "rules_proto_grpc_repos", "rules_proto_grpc_toolchains")

rules_proto_grpc_toolchains()

rules_proto_grpc_repos()

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

load("@rules_proto_grpc//cpp:repositories.bzl", rules_proto_grpc_cpp_repos = "cpp_repos")

rules_proto_grpc_cpp_repos()

#--------------------
# rules_foreign_cc
#--------------------
http_archive(
    name = "rules_foreign_cc",
    sha256 = "e14a159c452a68a97a7c59fa458033cc91edb8224516295b047a95555140af5f",
    strip_prefix = "rules_foreign_cc-0.4.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.4.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.4.0/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

#--------------------
# Rules_m4
#--------------------
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_m4",
    sha256 = "b0309baacfd1b736ed82dc2bb27b0ec38455a31a3d5d20f8d05e831ebeef1a8e",
    urls = ["https://github.com/jmillikin/rules_m4/releases/download/v0.2.2/rules_m4-v0.2.2.tar.xz"],
)

load("@rules_m4//m4:m4.bzl", "m4_register_toolchains")

m4_register_toolchains(version = "1.4.18")

#--------------------
# flex
#--------------------
http_archive(
    name = "rules_flex",
    sha256 = "f1685512937c2e33a7ebc4d5c6cf38ed282c2ce3b7a9c7c0b542db7e5db59d52",
    urls = ["https://github.com/jmillikin/rules_flex/releases/download/v0.2/rules_flex-v0.2.tar.xz"],
)

load("@rules_flex//flex:flex.bzl", "flex_register_toolchains")

flex_register_toolchains()

#--------------------
# Bison
#--------------------
http_archive(
    name = "rules_bison",
    sha256 = "6ee9b396f450ca9753c3283944f9a6015b61227f8386893fb59d593455141481",
    urls = ["https://github.com/jmillikin/rules_bison/releases/download/v0.2/rules_bison-v0.2.tar.xz"],
)

load("@rules_bison//bison:bison.bzl", "bison_register_toolchains")

bison_register_toolchains()

#--------------------
# libffi for Souffle
#--------------------
http_archive(
    name = "org_sourceware_libffi",
    build_file = "@//third_party:libffi.BUILD",
    sha256 = "653ffdfc67fbb865f39c7e5df2a071c0beb17206ebfb0a9ecb18a18f63f6b263",
    strip_prefix = "libffi-3.3-rc2",
    urls = ["https://github.com/libffi/libffi/releases/download/v3.3-rc2/libffi-3.3-rc2.tar.gz"],
)

#--------------------
# Souffle:
#--------------------
http_archive(
    name = "souffle",
    build_file = "@//third_party/souffle:BUILD.souffle",
    patch_args = ["-p0"],
    patches = [
        "@//third_party/souffle:remove_config.patch",
    ],
    sha256 = "60c7c2630a63e1388d5e19ceceff2ce5f265afe42451553a561e4e8864d570cc",
    strip_prefix = "souffle-2.3",
    urls = ["https://github.com/souffle-lang/souffle/archive/refs/tags/2.3.zip"],
)

#-------------------
# nodejs
#-------------------
http_archive(
    name = "build_bazel_rules_nodejs",
    sha256 = "0fa2d443571c9e02fcb7363a74ae591bdcce2dd76af8677a95965edf329d778a",
    urls = ["https://github.com/bazelbuild/rules_nodejs/releases/download/3.6.0/rules_nodejs-3.6.0.tar.gz"],
)

load("@build_bazel_rules_nodejs//:index.bzl", "node_repositories")

# NOTE: this rule installs nodejs, npm, and yarn, but does NOT install
# your npm dependencies into your node_modules folder.
# You must still run the package manager to do this.
node_repositories(package_json = ["//third_party/arcs/tools/manifest2proto:package.json"])

# Note: Required for abseil.
http_archive(
    name = "rules_cc",
    sha256 = "e4ab6ab1321977536835d9e66e24f869e0f02929a286b23f86b1ab97543f9007",
    strip_prefix = "rules_cc-fb624ff008196216a86e958e98fbd1a2615d7bbe",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/fb624ff008196216a86e958e98fbd1a2615d7bbe.zip"],
)

absl_commit_sha = "4a98f786820f72ff7483c46d14fe85f3310f1272"

http_archive(
    name = "com_google_absl",
    sha256 = "b029ed744500bcbdfd4ba6529613db46b3d050d5d45865d94780268f1b6e3b49",
    strip_prefix = "abseil-cpp-" + absl_commit_sha,
    urls = ["https://github.com/abseil/abseil-cpp/archive/" + absl_commit_sha + ".zip"],
)

# Bazel platform rules.
http_archive(
    name = "platforms",
    sha256 = "b601beaf841244de5c5a50d2b2eddd34839788000fa1be4260ce6603ca0d8eb7",
    strip_prefix = "platforms-98939346da932eef0b54cf808622f5bb0928f00b",
    urls = ["https://github.com/bazelbuild/platforms/archive/98939346da932eef0b54cf808622f5bb0928f00b.zip"],
)

#-------------------
# Rust
#-------------------
http_archive(
    name = "rules_rust",
    sha256 = "531bdd470728b61ce41cf7604dc4f9a115983e455d46ac1d0c1632f613ab9fc3",
    strip_prefix = "rules_rust-d8238877c0e552639d3e057aadd6bfcf37592408",
    urls = [
        # `main` branch as of 2021-08-23
        "https://github.com/bazelbuild/rules_rust/archive/d8238877c0e552639d3e057aadd6bfcf37592408.tar.gz",
    ],
)

load("@rules_rust//rust:repositories.bzl", "rust_repositories")

# TODO(#143): We need this specific repository for authorization logic.
rust_repositories(
    edition = "2018",
    iso_date = "2021-07-01",
    version = "nightly",
)

#--------------------
# Cargo Raze Crates
#--------------------
load("//third_party/cargo:crates.bzl", "raze_fetch_remote_crates")

raze_fetch_remote_crates()

# Buildifier

# buildifier is written in Go and hence needs rules_go to be built.
# See https://github.com/bazelbuild/rules_go for the up to date setup instructions.
http_archive(
    name = "io_bazel_rules_go",
    sha256 = "2b1641428dff9018f9e85c0384f03ec6c10660d935b750e3fa1492a281a53b0f",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_go/releases/download/v0.29.0/rules_go-v0.29.0.zip",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.29.0/rules_go-v0.29.0.zip",
    ],
)

load("@io_bazel_rules_go//go:deps.bzl", "go_register_toolchains", "go_rules_dependencies")

go_rules_dependencies()

go_register_toolchains(version = "1.17.2")

http_archive(
    name = "bazel_gazelle",
    sha256 = "de69a09dc70417580aabf20a28619bb3ef60d038470c7cf8442fafcf627c21cb",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-gazelle/releases/download/v0.24.0/bazel-gazelle-v0.24.0.tar.gz",
        "https://github.com/bazelbuild/bazel-gazelle/releases/download/v0.24.0/bazel-gazelle-v0.24.0.tar.gz",
    ],
)

load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")

# If you use WORKSPACE.bazel, use the following line instead of the bare gazelle_dependencies():
# gazelle_dependencies(go_repository_default_config = "@//:WORKSPACE.bazel")
gazelle_dependencies()

http_archive(
    name = "buildtools",
    sha256 = "d368c47bbfc055010f118efb2962987475418737e901f7782d2a966d1dc80296",
    strip_prefix = "buildtools-4.2.5",
    urls = [
        "https://github.com/bazelbuild/buildtools/archive/refs/tags/4.2.5.tar.gz",
    ],
)

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    sha256 = "9711de4935a68cbaba9ad79ac4e69a8028c8ef1446ede4469017b676cefe9140",
    strip_prefix = "bazel-compile-commands-extractor-ba1751e3bb9865df2d4661ca778b0cd4ffd1fb10",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/ba1751e3bb9865df2d4661ca778b0cd4ffd1fb10.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

#Antlr

http_archive(
    name = "rules_antlr",
    sha256 = "26e6a83c665cf6c1093b628b3a749071322f0f70305d12ede30909695ed85591",
    strip_prefix = "rules_antlr-0.5.0",
    urls = ["https://github.com/marcohu/rules_antlr/archive/0.5.0.tar.gz"],
)

load("@rules_antlr//antlr:repositories.bzl", "rules_antlr_dependencies")
load("@rules_antlr//antlr:lang.bzl", "CPP")

rules_antlr_dependencies("4.8")

http_archive(
    name = "antlr4_runtimes",
    build_file = "@//third_party/antlr:antlr.BUILD",
    sha256 = "efe4057d75ab48145d4683100fec7f77d7f87fa258707330cadd1f8e6f7eecae",
    strip_prefix = "antlr4-4.9.3",
    urls = ["https://github.com/antlr/antlr4/archive/4.9.3.tar.gz"],
)

re2_commit_hash = "e8cb5ecb8ee1066611aa937a42fa10514edf30fb"

# RE2
http_archive(
    name = "com_googlesource_code_re2",
    sha256 = "c5f46950cdf33175f0668f454d9b6b4fe1b5a71ffd9283213e77fb04461af099",
    strip_prefix = "re2-" + re2_commit_hash,
    urls = [
        "https://github.com/google/re2/archive/" + re2_commit_hash + ".tar.gz",
    ],
)

fuzztest_commit_hash = "4a8253b7299cf1764550054fae55f1b58a65bbaa"

# Fuzztest
http_archive(
    name = "fuzztest",
    sha256 = "72ae2d28323748d2c466396fa47038351ae81bedc50b8a3c6f1de6a9f02cde33",
    strip_prefix = "fuzztest-" + fuzztest_commit_hash,
    urls = ["https://github.com/google/fuzztest/archive/" + fuzztest_commit_hash + ".zip"],
)

#zetasql
zetasql_commit_hash = "177d495a064e38684c462cf883e22428273bd996"

http_archive(
    name = "com_google_zetasql",
    sha256 = "5dc8f45b5d2b6d7ed39546d1c43c1a7cc341e6c1ae36aa93d8663f9c3a475aee",
    strip_prefix = "zetasql-" + zetasql_commit_hash,
    urls = [
        "https://github.com/google/zetasql/archive/" + zetasql_commit_hash + ".zip",
    ],
)

load("@com_google_zetasql//bazel:zetasql_deps_step_1.bzl", "zetasql_deps_step_1")

zetasql_deps_step_1()

load("@com_google_zetasql//bazel:zetasql_deps_step_2.bzl", "zetasql_deps_step_2")

# To use ZetaSQL as a frontend, we only need the analyzer. Don't grab other
# dependencies unnecessarily.
zetasql_deps_step_2(
    analyzer_deps = True,
    evaluator_deps = False,
    java_deps = False,
    testing_deps = False,
    tools_deps = False,
)

load("@com_google_zetasql//bazel:zetasql_deps_step_3.bzl", "zetasql_deps_step_3")

zetasql_deps_step_3()

load("@com_google_zetasql//bazel:zetasql_deps_step_4.bzl", "zetasql_deps_step_4")

zetasql_deps_step_4()
