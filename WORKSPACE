load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

#--------------------
# googletest
#--------------------
http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
    urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
)

# Protobuf:
#
# (See https://github.com/rules-proto-grpc/rules_proto_grpc)
http_archive(
    name = "rules_proto_grpc",
    sha256 = "7954abbb6898830cd10ac9714fbcacf092299fda00ed2baf781172f545120419",
    strip_prefix = "rules_proto_grpc-3.1.1",
    urls = ["https://github.com/rules-proto-grpc/rules_proto_grpc/archive/3.1.1.tar.gz"],
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
http_archive(
    name = "rules_m4",
    sha256 = "c67fa9891bb19e9e6c1050003ba648d35383b8cb3c9572f397ad24040fb7f0eb",
    urls = ["https://github.com/jmillikin/rules_m4/releases/download/v0.2/rules_m4-v0.2.tar.xz"],
)

load("@rules_m4//m4:m4.bzl", "m4_register_toolchains")

m4_register_toolchains()

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
    sha256 = "654c1b33b2b3f20fdc1f0983dfed562c24a0baa230fd431401cc0004464c6b4d",
    strip_prefix = "souffle-fbb4c4b967bf58cccb7aca58e3d200a799218d98",
    urls = ["https://github.com/souffle-lang/souffle/archive/fbb4c4b967bf58cccb7aca58e3d200a799218d98.zip"],
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

http_archive(
    name = "absl",
    sha256 = "59b862f50e710277f8ede96f083a5bb8d7c9595376146838b9580be90374ee1f",
    strip_prefix = "abseil-cpp-20210324.2",
    urls = ["https://github.com/abseil/abseil-cpp/archive/refs/tags/20210324.2.tar.gz"],
)

# gflags, required for glog.
http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

# Google logger (glog).
http_archive(
    name = "glog",
    sha256 = "21bc744fb7f2fa701ee8db339ded7dce4f975d0d55837a97be7d46e8382dea5a",
    strip_prefix = "glog-0.5.0",
    urls = ["https://github.com/google/glog/archive/v0.5.0.zip"],
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
rust_repositories(version = "nightly", iso_date = "2021-07-01", edition="2018")

#--------------------
# Cargo Raze Crates
#--------------------
load("//third_party/cargo:crates.bzl", "raze_fetch_remote_crates")
raze_fetch_remote_crates()
