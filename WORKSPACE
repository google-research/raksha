load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

#--------------------
# googletest
#--------------------
http_archive(
    name = "com_google_googletest",
    sha256 = "8f1958a3986e646913e3e8160ca0761a31a8e1b298af11eb9e50d564f09e977b",
    strip_prefix = "googletest-71140c3ca7a87bb1b5b9c9f1500fea8858cce344",
    urls = ["https://github.com/google/googletest/archive/71140c3ca7a87bb1b5b9c9f1500fea8858cce344.zip"],
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

ANTLR_VERSION = "4.11.1"

http_archive(
    name = "antlr4_runtimes",
    build_file = "@//third_party/antlr:antlr.BUILD",
    sha256 = "81f87f03bb83b48da62e4fc8bfdaf447efb9fb3b7f19eb5cbc37f64e171218cf",
    strip_prefix = "antlr4-" + ANTLR_VERSION,
    urls = ["https://github.com/antlr/antlr4/archive/" + ANTLR_VERSION + ".tar.gz"],
)

RULES_ANTLR_COMMIT_HASH = "2e6a30d8ad24582a5cb89d2e42a93cfbfdca7002"

http_archive(
    name = "rules_antlr",
    sha256 = "971b338ee8508e44076002e433292df3e13ee8e775277f5740971d7a72cec409",
    strip_prefix = "rules_antlr-" + RULES_ANTLR_COMMIT_HASH,
    urls = ["https://github.com/markww/rules_antlr/archive/" + RULES_ANTLR_COMMIT_HASH + ".zip"],
)

load("@rules_antlr//antlr:repositories.bzl", "rules_antlr_dependencies")
load("@rules_antlr//antlr:lang.bzl", "CPP")

rules_antlr_dependencies(ANTLR_VERSION)

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

# SQLite Antlr grammar

SQLITE_ANTLR_COMMIT_HASH = "0037201886b6ec66bcb423c274cee593a64f5e25"

# Dealing with the two files that we want out of the ANTLR SQLite parser is
# surprisingly complicated. `exports_files` seems to not work well with `BUILD`
# files dropped into http archives, and we can't bring our antlr rules in
# without a circular dependency on sub repositories. Instead, just grab the two
# files we want directly.
http_file(
    name = "antlr4_sqlite_parser",
    sha256 = "8d8bdcc1e2376a16be37535fabec73cecdae0bf1ad76fbd8095914d6f9b00f65",
    urls = ["https://raw.githubusercontent.com/antlr/grammars-v4/" + SQLITE_ANTLR_COMMIT_HASH + "/sql/sqlite/SQLiteParser.g4"],
)

http_file(
    name = "antlr4_sqlite_lexer",
    sha256 = "6c153471bf3576cdaac3efcefbd4fba7946bf685abc62b15e3075dd14b8144e6",
    urls = ["https://raw.githubusercontent.com/antlr/grammars-v4/" + SQLITE_ANTLR_COMMIT_HASH + "/sql/sqlite/SQLiteLexer.g4"],
)
