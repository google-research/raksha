"""
@generated
cargo-raze generated Bazel file.

DO NOT EDIT! Replaced on runs of cargo-raze
"""

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")  # buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")  # buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")  # buildifier: disable=load

def raze_fetch_remote_crates():
    """This function defines a collection of repos and should be called in a WORKSPACE file"""
    maybe(
        http_archive,
        name = "raze__ansi_term__0_12_1",
        url = "https://crates.io/api/v1/crates/ansi_term/0.12.1/download",
        type = "tar.gz",
        strip_prefix = "ansi_term-0.12.1",
        build_file = Label("//third_party/cargo/remote:BUILD.ansi_term-0.12.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__antlr_rust__0_2_0",
        url = "https://crates.io/api/v1/crates/antlr-rust/0.2.0/download",
        type = "tar.gz",
        sha256 = "31cef0bab9c69cb4edd4764cd227b0db357a5634ad5a9200f9fb0a8d32e50947",
        strip_prefix = "antlr-rust-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.antlr-rust-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__anyhow__1_0_40",
        url = "https://crates.io/api/v1/crates/anyhow/1.0.40/download",
        type = "tar.gz",
        sha256 = "28b2cd92db5cbd74e8e5028f7e27dd7aa3090e89e4f2a197cc7c8dfb69c7063b",
        strip_prefix = "anyhow-1.0.40",
        build_file = Label("//third_party/cargo/remote:BUILD.anyhow-1.0.40.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__atty__0_2_14",
        url = "https://crates.io/api/v1/crates/atty/0.2.14/download",
        type = "tar.gz",
        strip_prefix = "atty-0.2.14",
        build_file = Label("//third_party/cargo/remote:BUILD.atty-0.2.14.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__autocfg__1_0_1",
        url = "https://crates.io/api/v1/crates/autocfg/1.0.1/download",
        type = "tar.gz",
        sha256 = "cdb031dd78e28731d87d56cc8ffef4a8f36ca26c38fe2de700543e627f8a464a",
        strip_prefix = "autocfg-1.0.1",
        build_file = Label("//third_party/cargo/remote:BUILD.autocfg-1.0.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__base64__0_13_0",
        url = "https://crates.io/api/v1/crates/base64/0.13.0/download",
        type = "tar.gz",
        sha256 = "904dfeac50f3cdaba28fc6f57fdcddb75f49ed61346676a78c4ffe55877802fd",
        strip_prefix = "base64-0.13.0",
        build_file = Label("//third_party/cargo/remote:BUILD.base64-0.13.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__better_any__0_1_1",
        url = "https://crates.io/api/v1/crates/better_any/0.1.1/download",
        type = "tar.gz",
        sha256 = "b359aebd937c17c725e19efcb661200883f04c49c53e7132224dac26da39d4a0",
        strip_prefix = "better_any-0.1.1",
        build_file = Label("//third_party/cargo/remote:BUILD.better_any-0.1.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__better_typeid_derive__0_1_1",
        url = "https://crates.io/api/v1/crates/better_typeid_derive/0.1.1/download",
        type = "tar.gz",
        sha256 = "3deeecb812ca5300b7d3f66f730cc2ebd3511c3d36c691dd79c165d5b19a26e3",
        strip_prefix = "better_typeid_derive-0.1.1",
        build_file = Label("//third_party/cargo/remote:BUILD.better_typeid_derive-0.1.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__bincode__1_3_3",
        url = "https://crates.io/api/v1/crates/bincode/1.3.3/download",
        type = "tar.gz",
        sha256 = "b1f45e9417d87227c7a56d22e471c6206462cba514c7590c09aff4cf6d1ddcad",
        strip_prefix = "bincode-1.3.3",
        build_file = Label("//third_party/cargo/remote:BUILD.bincode-1.3.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__bit_set__0_5_2",
        url = "https://crates.io/api/v1/crates/bit-set/0.5.2/download",
        type = "tar.gz",
        sha256 = "6e11e16035ea35e4e5997b393eacbf6f63983188f7a2ad25bfb13465f5ad59de",
        strip_prefix = "bit-set-0.5.2",
        build_file = Label("//third_party/cargo/remote:BUILD.bit-set-0.5.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__bit_vec__0_6_3",
        url = "https://crates.io/api/v1/crates/bit-vec/0.6.3/download",
        type = "tar.gz",
        sha256 = "349f9b6a179ed607305526ca489b34ad0a41aed5f7980fa90eb03160b69598fb",
        strip_prefix = "bit-vec-0.6.3",
        build_file = Label("//third_party/cargo/remote:BUILD.bit-vec-0.6.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__bitflags__1_2_1",
        url = "https://crates.io/api/v1/crates/bitflags/1.2.1/download",
        type = "tar.gz",
        sha256 = "cf1de2fe8c75bc145a2f577add951f8134889b4795d47466a54a5c846d691693",
        strip_prefix = "bitflags-1.2.1",
        build_file = Label("//third_party/cargo/remote:BUILD.bitflags-1.2.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__bitvec__0_20_4",
        url = "https://crates.io/api/v1/crates/bitvec/0.20.4/download",
        type = "tar.gz",
        sha256 = "7774144344a4faa177370406a7ff5f1da24303817368584c6206c8303eb07848",
        strip_prefix = "bitvec-0.20.4",
        build_file = Label("//third_party/cargo/remote:BUILD.bitvec-0.20.4.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__block_buffer__0_9_0",
        url = "https://crates.io/api/v1/crates/block-buffer/0.9.0/download",
        type = "tar.gz",
        sha256 = "4152116fd6e9dadb291ae18fc1ec3575ed6d84c29642d97890f4b4a3417297e4",
        strip_prefix = "block-buffer-0.9.0",
        build_file = Label("//third_party/cargo/remote:BUILD.block-buffer-0.9.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__byteorder__1_4_3",
        url = "https://crates.io/api/v1/crates/byteorder/1.4.3/download",
        type = "tar.gz",
        sha256 = "14c189c53d098945499cdfa7ecc63567cf3886b3332b312a5b4585d8d3a6a610",
        strip_prefix = "byteorder-1.4.3",
        build_file = Label("//third_party/cargo/remote:BUILD.byteorder-1.4.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__bytes__1_0_1",
        url = "https://crates.io/api/v1/crates/bytes/1.0.1/download",
        type = "tar.gz",
        sha256 = "b700ce4376041dcd0a327fd0097c41095743c4c8af8887265942faf1100bd040",
        strip_prefix = "bytes-1.0.1",
        build_file = Label("//third_party/cargo/remote:BUILD.bytes-1.0.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__cfg_if__1_0_0",
        url = "https://crates.io/api/v1/crates/cfg-if/1.0.0/download",
        type = "tar.gz",
        sha256 = "baf1de4339761588bc0619e3cbc0120ee582ebb74b53b4efbf79117bd2da40fd",
        strip_prefix = "cfg-if-1.0.0",
        build_file = Label("//third_party/cargo/remote:BUILD.cfg-if-1.0.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__clap__2_34_0",
        url = "https://crates.io/api/v1/crates/clap/2.34.0/download",
        type = "tar.gz",
        strip_prefix = "clap-2.34.0",
        build_file = Label("//third_party/cargo/remote:BUILD.clap-2.34.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__const_oid__0_5_2",
        url = "https://crates.io/api/v1/crates/const-oid/0.5.2/download",
        type = "tar.gz",
        sha256 = "279bc8fc53f788a75c7804af68237d1fce02cde1e275a886a4b320604dc2aeda",
        strip_prefix = "const-oid-0.5.2",
        build_file = Label("//third_party/cargo/remote:BUILD.const-oid-0.5.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__cpufeatures__0_1_4",
        url = "https://crates.io/api/v1/crates/cpufeatures/0.1.4/download",
        type = "tar.gz",
        sha256 = "ed00c67cb5d0a7d64a44f6ad2668db7e7530311dd53ea79bcd4fb022c64911c8",
        strip_prefix = "cpufeatures-0.1.4",
        build_file = Label("//third_party/cargo/remote:BUILD.cpufeatures-0.1.4.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__crypto_mac__0_11_0",
        url = "https://crates.io/api/v1/crates/crypto-mac/0.11.0/download",
        type = "tar.gz",
        sha256 = "25fab6889090c8133f3deb8f73ba3c65a7f456f66436fc012a1b1e272b1e103e",
        strip_prefix = "crypto-mac-0.11.0",
        build_file = Label("//third_party/cargo/remote:BUILD.crypto-mac-0.11.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__curve25519_dalek__3_1_0",
        url = "https://crates.io/api/v1/crates/curve25519-dalek/3.1.0/download",
        type = "tar.gz",
        sha256 = "639891fde0dbea823fc3d798a0fdf9d2f9440a42d64a78ab3488b0ca025117b3",
        strip_prefix = "curve25519-dalek-3.1.0",
        build_file = Label("//third_party/cargo/remote:BUILD.curve25519-dalek-3.1.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__der__0_3_5",
        url = "https://crates.io/api/v1/crates/der/0.3.5/download",
        type = "tar.gz",
        sha256 = "2eeb9d92785d1facb50567852ce75d0858630630e7eabea59cf7eb7474051087",
        strip_prefix = "der-0.3.5",
        build_file = Label("//third_party/cargo/remote:BUILD.der-0.3.5.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__digest__0_9_0",
        url = "https://crates.io/api/v1/crates/digest/0.9.0/download",
        type = "tar.gz",
        sha256 = "d3dd60d1080a57a05ab032377049e0591415d2b31afd7028356dbf3cc6dcb066",
        strip_prefix = "digest-0.9.0",
        build_file = Label("//third_party/cargo/remote:BUILD.digest-0.9.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ecdsa__0_11_1",
        url = "https://crates.io/api/v1/crates/ecdsa/0.11.1/download",
        type = "tar.gz",
        sha256 = "34d33b390ab82f2e1481e331dbd0530895640179d2128ef9a79cc690b78d1eba",
        strip_prefix = "ecdsa-0.11.1",
        build_file = Label("//third_party/cargo/remote:BUILD.ecdsa-0.11.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ed25519__1_1_1",
        url = "https://crates.io/api/v1/crates/ed25519/1.1.1/download",
        type = "tar.gz",
        sha256 = "8d0860415b12243916284c67a9be413e044ee6668247b99ba26d94b2bc06c8f6",
        strip_prefix = "ed25519-1.1.1",
        build_file = Label("//third_party/cargo/remote:BUILD.ed25519-1.1.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ed25519_dalek__1_0_1",
        url = "https://crates.io/api/v1/crates/ed25519-dalek/1.0.1/download",
        type = "tar.gz",
        sha256 = "c762bae6dcaf24c4c84667b8579785430908723d5c889f469d76a41d59cc7a9d",
        strip_prefix = "ed25519-dalek-1.0.1",
        build_file = Label("//third_party/cargo/remote:BUILD.ed25519-dalek-1.0.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__either__1_6_1",
        url = "https://crates.io/api/v1/crates/either/1.6.1/download",
        type = "tar.gz",
        sha256 = "e78d4f1cc4ae33bbfc157ed5d5a5ef3bc29227303d595861deb238fcec4e9457",
        strip_prefix = "either-1.6.1",
        build_file = Label("//third_party/cargo/remote:BUILD.either-1.6.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__elliptic_curve__0_9_12",
        url = "https://crates.io/api/v1/crates/elliptic-curve/0.9.12/download",
        type = "tar.gz",
        sha256 = "c13e9b0c3c4170dcc2a12783746c4205d98e18957f57854251eea3f9750fe005",
        strip_prefix = "elliptic-curve-0.9.12",
        build_file = Label("//third_party/cargo/remote:BUILD.elliptic-curve-0.9.12.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ff__0_9_0",
        url = "https://crates.io/api/v1/crates/ff/0.9.0/download",
        type = "tar.gz",
        sha256 = "72a4d941a5b7c2a75222e2d44fcdf634a67133d9db31e177ae5ff6ecda852bfe",
        strip_prefix = "ff-0.9.0",
        build_file = Label("//third_party/cargo/remote:BUILD.ff-0.9.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__fixedbitset__0_2_0",
        url = "https://crates.io/api/v1/crates/fixedbitset/0.2.0/download",
        type = "tar.gz",
        sha256 = "37ab347416e802de484e4d03c7316c48f1ecb56574dfd4a46a80f173ce1de04d",
        strip_prefix = "fixedbitset-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.fixedbitset-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__funty__1_1_0",
        url = "https://crates.io/api/v1/crates/funty/1.1.0/download",
        type = "tar.gz",
        sha256 = "fed34cd105917e91daa4da6b3728c47b068749d6a62c59811f06ed2ac71d9da7",
        strip_prefix = "funty-1.1.0",
        build_file = Label("//third_party/cargo/remote:BUILD.funty-1.1.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__generic_array__0_14_4",
        url = "https://crates.io/api/v1/crates/generic-array/0.14.4/download",
        type = "tar.gz",
        sha256 = "501466ecc8a30d1d3b7fc9229b122b2ce8ed6e9d9223f1138d4babb253e51817",
        strip_prefix = "generic-array-0.14.4",
        build_file = Label("//third_party/cargo/remote:BUILD.generic-array-0.14.4.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__getrandom__0_1_16",
        url = "https://crates.io/api/v1/crates/getrandom/0.1.16/download",
        type = "tar.gz",
        sha256 = "8fc3cb4d91f53b50155bdcfd23f6a4c39ae1969c2ae85982b135750cccaf5fce",
        strip_prefix = "getrandom-0.1.16",
        build_file = Label("//third_party/cargo/remote:BUILD.getrandom-0.1.16.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__getrandom__0_2_3",
        url = "https://crates.io/api/v1/crates/getrandom/0.2.3/download",
        type = "tar.gz",
        sha256 = "7fcd999463524c52659517fe2cea98493cfe485d10565e7b0fb07dbba7ad2753",
        strip_prefix = "getrandom-0.2.3",
        build_file = Label("//third_party/cargo/remote:BUILD.getrandom-0.2.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__group__0_9_0",
        url = "https://crates.io/api/v1/crates/group/0.9.0/download",
        type = "tar.gz",
        sha256 = "61b3c1e8b4f1ca07e6605ea1be903a5f6956aec5c8a67fd44d56076631675ed8",
        strip_prefix = "group-0.9.0",
        build_file = Label("//third_party/cargo/remote:BUILD.group-0.9.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__hashbrown__0_9_1",
        url = "https://crates.io/api/v1/crates/hashbrown/0.9.1/download",
        type = "tar.gz",
        sha256 = "d7afe4a420e3fe79967a00898cc1f4db7c8a49a9333a29f8a4bd76a253d5cd04",
        strip_prefix = "hashbrown-0.9.1",
        build_file = Label("//third_party/cargo/remote:BUILD.hashbrown-0.9.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__heck__0_3_2",
        url = "https://crates.io/api/v1/crates/heck/0.3.2/download",
        type = "tar.gz",
        sha256 = "87cbf45460356b7deeb5e3415b5563308c0a9b057c85e12b06ad551f98d0a6ac",
        strip_prefix = "heck-0.3.2",
        build_file = Label("//third_party/cargo/remote:BUILD.heck-0.3.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__hermit_abi__0_1_19",
        url = "https://crates.io/api/v1/crates/hermit-abi/0.1.19/download",
        type = "tar.gz",
        strip_prefix = "hermit-abi-0.1.19",
        build_file = Label("//third_party/cargo/remote:BUILD.hermit-abi-0.1.19.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__hkdf__0_11_0",
        url = "https://crates.io/api/v1/crates/hkdf/0.11.0/download",
        type = "tar.gz",
        sha256 = "01706d578d5c281058480e673ae4086a9f4710d8df1ad80a5b03e39ece5f886b",
        strip_prefix = "hkdf-0.11.0",
        build_file = Label("//third_party/cargo/remote:BUILD.hkdf-0.11.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__hmac__0_11_0",
        url = "https://crates.io/api/v1/crates/hmac/0.11.0/download",
        type = "tar.gz",
        sha256 = "2a2a2320eb7ec0ebe8da8f744d7812d9fc4cb4d09344ac01898dbcb6a20ae69b",
        strip_prefix = "hmac-0.11.0",
        build_file = Label("//third_party/cargo/remote:BUILD.hmac-0.11.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__indexmap__1_6_2",
        url = "https://crates.io/api/v1/crates/indexmap/1.6.2/download",
        type = "tar.gz",
        sha256 = "824845a0bf897a9042383849b02c1bc219c2383772efcd5c6f9766fa4b81aef3",
        strip_prefix = "indexmap-1.6.2",
        build_file = Label("//third_party/cargo/remote:BUILD.indexmap-1.6.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__instant__0_1_9",
        url = "https://crates.io/api/v1/crates/instant/0.1.9/download",
        type = "tar.gz",
        sha256 = "61124eeebbd69b8190558df225adf7e4caafce0d743919e5d6b19652314ec5ec",
        strip_prefix = "instant-0.1.9",
        build_file = Label("//third_party/cargo/remote:BUILD.instant-0.1.9.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__itertools__0_9_0",
        url = "https://crates.io/api/v1/crates/itertools/0.9.0/download",
        type = "tar.gz",
        sha256 = "284f18f85651fe11e8a991b2adb42cb078325c996ed026d994719efcfca1d54b",
        strip_prefix = "itertools-0.9.0",
        build_file = Label("//third_party/cargo/remote:BUILD.itertools-0.9.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__itoa__0_4_7",
        url = "https://crates.io/api/v1/crates/itoa/0.4.7/download",
        type = "tar.gz",
        sha256 = "dd25036021b0de88a0aff6b850051563c6516d0bf53f8638938edbb9de732736",
        strip_prefix = "itoa-0.4.7",
        build_file = Label("//third_party/cargo/remote:BUILD.itoa-0.4.7.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__lazy_static__1_4_0",
        url = "https://crates.io/api/v1/crates/lazy_static/1.4.0/download",
        type = "tar.gz",
        sha256 = "e2abad23fbc42b3700f2f279844dc832adb2b2eb069b2df918f455c4e18cc646",
        strip_prefix = "lazy_static-1.4.0",
        build_file = Label("//third_party/cargo/remote:BUILD.lazy_static-1.4.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__libc__0_2_93",
        url = "https://crates.io/api/v1/crates/libc/0.2.93/download",
        type = "tar.gz",
        sha256 = "9385f66bf6105b241aa65a61cb923ef20efc665cb9f9bb50ac2f0c4b7f378d41",
        strip_prefix = "libc-0.2.93",
        build_file = Label("//third_party/cargo/remote:BUILD.libc-0.2.93.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__lock_api__0_4_3",
        url = "https://crates.io/api/v1/crates/lock_api/0.4.3/download",
        type = "tar.gz",
        sha256 = "5a3c91c24eae6777794bb1997ad98bbb87daf92890acab859f7eaa4320333176",
        strip_prefix = "lock_api-0.4.3",
        build_file = Label("//third_party/cargo/remote:BUILD.lock_api-0.4.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__log__0_4_14",
        url = "https://crates.io/api/v1/crates/log/0.4.14/download",
        type = "tar.gz",
        sha256 = "51b9bbe6c47d51fc3e1a9b945965946b4c44142ab8792c50835a980d362c2710",
        strip_prefix = "log-0.4.14",
        build_file = Label("//third_party/cargo/remote:BUILD.log-0.4.14.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__multimap__0_8_3",
        url = "https://crates.io/api/v1/crates/multimap/0.8.3/download",
        type = "tar.gz",
        sha256 = "e5ce46fe64a9d73be07dcbe690a38ce1b293be448fd8ce1e6c1b8062c9f72c6a",
        strip_prefix = "multimap-0.8.3",
        build_file = Label("//third_party/cargo/remote:BUILD.multimap-0.8.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__murmur3__0_4_1",
        url = "https://crates.io/api/v1/crates/murmur3/0.4.1/download",
        type = "tar.gz",
        sha256 = "a198f9589efc03f544388dfc4a19fe8af4323662b62f598b8dcfdac62c14771c",
        strip_prefix = "murmur3-0.4.1",
        build_file = Label("//third_party/cargo/remote:BUILD.murmur3-0.4.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__once_cell__1_7_2",
        url = "https://crates.io/api/v1/crates/once_cell/1.7.2/download",
        type = "tar.gz",
        sha256 = "af8b08b04175473088b46763e51ee54da5f9a164bc162f615b91bc179dbf15a3",
        strip_prefix = "once_cell-1.7.2",
        build_file = Label("//third_party/cargo/remote:BUILD.once_cell-1.7.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__opaque_debug__0_3_0",
        url = "https://crates.io/api/v1/crates/opaque-debug/0.3.0/download",
        type = "tar.gz",
        sha256 = "624a8340c38c1b80fd549087862da4ba43e08858af025b236e509b6649fc13d5",
        strip_prefix = "opaque-debug-0.3.0",
        build_file = Label("//third_party/cargo/remote:BUILD.opaque-debug-0.3.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__p256__0_8_1",
        url = "https://crates.io/api/v1/crates/p256/0.8.1/download",
        type = "tar.gz",
        sha256 = "2f05f5287453297c4c16af5e2b04df8fd2a3008d70f252729650bc6d7ace5844",
        strip_prefix = "p256-0.8.1",
        build_file = Label("//third_party/cargo/remote:BUILD.p256-0.8.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__parking_lot__0_11_1",
        url = "https://crates.io/api/v1/crates/parking_lot/0.11.1/download",
        type = "tar.gz",
        sha256 = "6d7744ac029df22dca6284efe4e898991d28e3085c706c972bcd7da4a27a15eb",
        strip_prefix = "parking_lot-0.11.1",
        build_file = Label("//third_party/cargo/remote:BUILD.parking_lot-0.11.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__parking_lot_core__0_8_3",
        url = "https://crates.io/api/v1/crates/parking_lot_core/0.8.3/download",
        type = "tar.gz",
        sha256 = "fa7a782938e745763fe6907fc6ba86946d72f49fe7e21de074e08128a99fb018",
        strip_prefix = "parking_lot_core-0.8.3",
        build_file = Label("//third_party/cargo/remote:BUILD.parking_lot_core-0.8.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__petgraph__0_5_1",
        url = "https://crates.io/api/v1/crates/petgraph/0.5.1/download",
        type = "tar.gz",
        sha256 = "467d164a6de56270bd7c4d070df81d07beace25012d5103ced4e9ff08d6afdb7",
        strip_prefix = "petgraph-0.5.1",
        build_file = Label("//third_party/cargo/remote:BUILD.petgraph-0.5.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__pkcs8__0_6_1",
        url = "https://crates.io/api/v1/crates/pkcs8/0.6.1/download",
        type = "tar.gz",
        sha256 = "c9c2f795bc591cb3384cb64082a578b89207ac92bb89c9d98c1ea2ace7cd8110",
        strip_prefix = "pkcs8-0.6.1",
        build_file = Label("//third_party/cargo/remote:BUILD.pkcs8-0.6.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ppv_lite86__0_2_10",
        url = "https://crates.io/api/v1/crates/ppv-lite86/0.2.10/download",
        type = "tar.gz",
        sha256 = "ac74c624d6b2d21f425f752262f42188365d7b8ff1aff74c82e45136510a4857",
        strip_prefix = "ppv-lite86-0.2.10",
        build_file = Label("//third_party/cargo/remote:BUILD.ppv-lite86-0.2.10.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__proc_macro_error__1_0_4",
        url = "https://crates.io/api/v1/crates/proc-macro-error/1.0.4/download",
        type = "tar.gz",
        strip_prefix = "proc-macro-error-1.0.4",
        build_file = Label("//third_party/cargo/remote:BUILD.proc-macro-error-1.0.4.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__proc_macro_error_attr__1_0_4",
        url = "https://crates.io/api/v1/crates/proc-macro-error-attr/1.0.4/download",
        type = "tar.gz",
        strip_prefix = "proc-macro-error-attr-1.0.4",
        build_file = Label("//third_party/cargo/remote:BUILD.proc-macro-error-attr-1.0.4.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__proc_macro2__1_0_26",
        url = "https://crates.io/api/v1/crates/proc-macro2/1.0.26/download",
        type = "tar.gz",
        sha256 = "a152013215dca273577e18d2bf00fa862b89b24169fb78c4c95aeb07992c9cec",
        strip_prefix = "proc-macro2-1.0.26",
        build_file = Label("//third_party/cargo/remote:BUILD.proc-macro2-1.0.26.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__prost__0_7_0",
        url = "https://crates.io/api/v1/crates/prost/0.7.0/download",
        type = "tar.gz",
        sha256 = "9e6984d2f1a23009bd270b8bb56d0926810a3d483f59c987d77969e9d8e840b2",
        strip_prefix = "prost-0.7.0",
        build_file = Label("//third_party/cargo/remote:BUILD.prost-0.7.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__prost_build__0_7_0",
        url = "https://crates.io/api/v1/crates/prost-build/0.7.0/download",
        type = "tar.gz",
        sha256 = "32d3ebd75ac2679c2af3a92246639f9fcc8a442ee420719cc4fe195b98dd5fa3",
        strip_prefix = "prost-build-0.7.0",
        build_file = Label("//third_party/cargo/remote:BUILD.prost-build-0.7.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__prost_derive__0_7_0",
        url = "https://crates.io/api/v1/crates/prost-derive/0.7.0/download",
        type = "tar.gz",
        sha256 = "169a15f3008ecb5160cba7d37bcd690a7601b6d30cfb87a117d45e59d52af5d4",
        strip_prefix = "prost-derive-0.7.0",
        build_file = Label("//third_party/cargo/remote:BUILD.prost-derive-0.7.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__prost_types__0_7_0",
        url = "https://crates.io/api/v1/crates/prost-types/0.7.0/download",
        type = "tar.gz",
        sha256 = "b518d7cdd93dab1d1122cf07fa9a60771836c668dde9d9e2a139f957f0d9f1bb",
        strip_prefix = "prost-types-0.7.0",
        build_file = Label("//third_party/cargo/remote:BUILD.prost-types-0.7.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__quote__1_0_9",
        url = "https://crates.io/api/v1/crates/quote/1.0.9/download",
        type = "tar.gz",
        sha256 = "c3d0b9745dc2debf507c8422de05d7226cc1f0644216dfdfead988f9b1ab32a7",
        strip_prefix = "quote-1.0.9",
        build_file = Label("//third_party/cargo/remote:BUILD.quote-1.0.9.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__radium__0_6_2",
        url = "https://crates.io/api/v1/crates/radium/0.6.2/download",
        type = "tar.gz",
        sha256 = "643f8f41a8ebc4c5dc4515c82bb8abd397b527fc20fd681b7c011c2aee5d44fb",
        strip_prefix = "radium-0.6.2",
        build_file = Label("//third_party/cargo/remote:BUILD.radium-0.6.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand__0_7_3",
        url = "https://crates.io/api/v1/crates/rand/0.7.3/download",
        type = "tar.gz",
        sha256 = "6a6b1679d49b24bbfe0c803429aa1874472f50d9b363131f0e89fc356b544d03",
        strip_prefix = "rand-0.7.3",
        build_file = Label("//third_party/cargo/remote:BUILD.rand-0.7.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand__0_8_3",
        url = "https://crates.io/api/v1/crates/rand/0.8.3/download",
        type = "tar.gz",
        sha256 = "0ef9e7e66b4468674bfcb0c81af8b7fa0bb154fa9f28eb840da5c447baeb8d7e",
        strip_prefix = "rand-0.8.3",
        build_file = Label("//third_party/cargo/remote:BUILD.rand-0.8.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand_chacha__0_2_2",
        url = "https://crates.io/api/v1/crates/rand_chacha/0.2.2/download",
        type = "tar.gz",
        sha256 = "f4c8ed856279c9737206bf725bf36935d8666ead7aa69b52be55af369d193402",
        strip_prefix = "rand_chacha-0.2.2",
        build_file = Label("//third_party/cargo/remote:BUILD.rand_chacha-0.2.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand_chacha__0_3_0",
        url = "https://crates.io/api/v1/crates/rand_chacha/0.3.0/download",
        type = "tar.gz",
        sha256 = "e12735cf05c9e10bf21534da50a147b924d555dc7a547c42e6bb2d5b6017ae0d",
        strip_prefix = "rand_chacha-0.3.0",
        build_file = Label("//third_party/cargo/remote:BUILD.rand_chacha-0.3.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand_core__0_5_1",
        url = "https://crates.io/api/v1/crates/rand_core/0.5.1/download",
        type = "tar.gz",
        sha256 = "90bde5296fc891b0cef12a6d03ddccc162ce7b2aff54160af9338f8d40df6d19",
        strip_prefix = "rand_core-0.5.1",
        build_file = Label("//third_party/cargo/remote:BUILD.rand_core-0.5.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand_core__0_6_2",
        url = "https://crates.io/api/v1/crates/rand_core/0.6.2/download",
        type = "tar.gz",
        sha256 = "34cf66eb183df1c5876e2dcf6b13d57340741e8dc255b48e40a26de954d06ae7",
        strip_prefix = "rand_core-0.6.2",
        build_file = Label("//third_party/cargo/remote:BUILD.rand_core-0.6.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand_hc__0_2_0",
        url = "https://crates.io/api/v1/crates/rand_hc/0.2.0/download",
        type = "tar.gz",
        sha256 = "ca3129af7b92a17112d59ad498c6f81eaf463253766b90396d39ea7a39d6613c",
        strip_prefix = "rand_hc-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.rand_hc-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__rand_hc__0_3_0",
        url = "https://crates.io/api/v1/crates/rand_hc/0.3.0/download",
        type = "tar.gz",
        sha256 = "3190ef7066a446f2e7f42e239d161e905420ccab01eb967c9eb27d21b2322a73",
        strip_prefix = "rand_hc-0.3.0",
        build_file = Label("//third_party/cargo/remote:BUILD.rand_hc-0.3.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__redox_syscall__0_2_6",
        url = "https://crates.io/api/v1/crates/redox_syscall/0.2.6/download",
        type = "tar.gz",
        sha256 = "8270314b5ccceb518e7e578952f0b72b88222d02e8f77f5ecf7abbb673539041",
        strip_prefix = "redox_syscall-0.2.6",
        build_file = Label("//third_party/cargo/remote:BUILD.redox_syscall-0.2.6.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__remove_dir_all__0_5_3",
        url = "https://crates.io/api/v1/crates/remove_dir_all/0.5.3/download",
        type = "tar.gz",
        sha256 = "3acd125665422973a33ac9d3dd2df85edad0f4ae9b00dafb1a05e43a9f5ef8e7",
        strip_prefix = "remove_dir_all-0.5.3",
        build_file = Label("//third_party/cargo/remote:BUILD.remove_dir_all-0.5.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__ryu__1_0_5",
        url = "https://crates.io/api/v1/crates/ryu/1.0.5/download",
        type = "tar.gz",
        sha256 = "71d301d4193d031abdd79ff7e3dd721168a9572ef3fe51a1517aba235bd8f86e",
        strip_prefix = "ryu-1.0.5",
        build_file = Label("//third_party/cargo/remote:BUILD.ryu-1.0.5.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__scopeguard__1_1_0",
        url = "https://crates.io/api/v1/crates/scopeguard/1.1.0/download",
        type = "tar.gz",
        sha256 = "d29ab0c6d3fc0ee92fe66e2d99f700eab17a8d57d1c1d3b748380fb20baa78cd",
        strip_prefix = "scopeguard-1.1.0",
        build_file = Label("//third_party/cargo/remote:BUILD.scopeguard-1.1.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__serde__1_0_126",
        url = "https://crates.io/api/v1/crates/serde/1.0.126/download",
        type = "tar.gz",
        sha256 = "ec7505abeacaec74ae4778d9d9328fe5a5d04253220a85c4ee022239fc996d03",
        strip_prefix = "serde-1.0.126",
        build_file = Label("//third_party/cargo/remote:BUILD.serde-1.0.126.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__serde_derive__1_0_126",
        url = "https://crates.io/api/v1/crates/serde_derive/1.0.126/download",
        type = "tar.gz",
        sha256 = "963a7dbc9895aeac7ac90e74f34a5d5261828f79df35cbed41e10189d3804d43",
        strip_prefix = "serde_derive-1.0.126",
        build_file = Label("//third_party/cargo/remote:BUILD.serde_derive-1.0.126.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__serde_json__1_0_64",
        url = "https://crates.io/api/v1/crates/serde_json/1.0.64/download",
        type = "tar.gz",
        sha256 = "799e97dc9fdae36a5c8b8f2cae9ce2ee9fdce2058c57a93e6099d919fd982f79",
        strip_prefix = "serde_json-1.0.64",
        build_file = Label("//third_party/cargo/remote:BUILD.serde_json-1.0.64.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__sha_1__0_9_6",
        url = "https://crates.io/api/v1/crates/sha-1/0.9.6/download",
        type = "tar.gz",
        sha256 = "8c4cfa741c5832d0ef7fab46cabed29c2aae926db0b11bb2069edd8db5e64e16",
        strip_prefix = "sha-1-0.9.6",
        build_file = Label("//third_party/cargo/remote:BUILD.sha-1-0.9.6.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__sha2__0_9_5",
        url = "https://crates.io/api/v1/crates/sha2/0.9.5/download",
        type = "tar.gz",
        sha256 = "b362ae5752fd2137731f9fa25fd4d9058af34666ca1966fb969119cc35719f12",
        strip_prefix = "sha2-0.9.5",
        build_file = Label("//third_party/cargo/remote:BUILD.sha2-0.9.5.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__signature__1_3_0",
        url = "https://crates.io/api/v1/crates/signature/1.3.0/download",
        type = "tar.gz",
        sha256 = "0f0242b8e50dd9accdd56170e94ca1ebd223b098eb9c83539a6e367d0f36ae68",
        strip_prefix = "signature-1.3.0",
        build_file = Label("//third_party/cargo/remote:BUILD.signature-1.3.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__smallvec__1_6_1",
        url = "https://crates.io/api/v1/crates/smallvec/1.6.1/download",
        type = "tar.gz",
        sha256 = "fe0f37c9e8f3c5a4a66ad655a93c74daac4ad00c441533bf5c6e7990bb42604e",
        strip_prefix = "smallvec-1.6.1",
        build_file = Label("//third_party/cargo/remote:BUILD.smallvec-1.6.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__spki__0_3_0",
        url = "https://crates.io/api/v1/crates/spki/0.3.0/download",
        type = "tar.gz",
        sha256 = "9dae7e047abc519c96350e9484a96c6bf1492348af912fd3446dd2dc323f6268",
        strip_prefix = "spki-0.3.0",
        build_file = Label("//third_party/cargo/remote:BUILD.spki-0.3.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__strsim__0_8_0",
        url = "https://crates.io/api/v1/crates/strsim/0.8.0/download",
        type = "tar.gz",
        strip_prefix = "strsim-0.8.0",
        build_file = Label("//third_party/cargo/remote:BUILD.strsim-0.8.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__structopt__0_3_25",
        url = "https://crates.io/api/v1/crates/structopt/0.3.25/download",
        type = "tar.gz",
        strip_prefix = "structopt-0.3.25",
        build_file = Label("//third_party/cargo/remote:BUILD.structopt-0.3.25.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__structopt_derive__0_4_18",
        url = "https://crates.io/api/v1/crates/structopt-derive/0.4.18/download",
        type = "tar.gz",
        strip_prefix = "structopt-derive-0.4.18",
        build_file = Label("//third_party/cargo/remote:BUILD.structopt-derive-0.4.18.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__subtle__2_4_0",
        url = "https://crates.io/api/v1/crates/subtle/2.4.0/download",
        type = "tar.gz",
        sha256 = "1e81da0851ada1f3e9d4312c704aa4f8806f0f9d69faaf8df2f3464b4a9437c2",
        strip_prefix = "subtle-2.4.0",
        build_file = Label("//third_party/cargo/remote:BUILD.subtle-2.4.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__syn__1_0_69",
        url = "https://crates.io/api/v1/crates/syn/1.0.69/download",
        type = "tar.gz",
        sha256 = "48fe99c6bd8b1cc636890bcc071842de909d902c81ac7dab53ba33c421ab8ffb",
        strip_prefix = "syn-1.0.69",
        build_file = Label("//third_party/cargo/remote:BUILD.syn-1.0.69.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__synstructure__0_12_4",
        url = "https://crates.io/api/v1/crates/synstructure/0.12.4/download",
        type = "tar.gz",
        sha256 = "b834f2d66f734cb897113e34aaff2f1ab4719ca946f9a7358dba8f8064148701",
        strip_prefix = "synstructure-0.12.4",
        build_file = Label("//third_party/cargo/remote:BUILD.synstructure-0.12.4.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__tap__1_0_1",
        url = "https://crates.io/api/v1/crates/tap/1.0.1/download",
        type = "tar.gz",
        sha256 = "55937e1799185b12863d447f42597ed69d9928686b8d88a1df17376a097d8369",
        strip_prefix = "tap-1.0.1",
        build_file = Label("//third_party/cargo/remote:BUILD.tap-1.0.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__tempfile__3_2_0",
        url = "https://crates.io/api/v1/crates/tempfile/3.2.0/download",
        type = "tar.gz",
        sha256 = "dac1c663cfc93810f88aed9b8941d48cabf856a1b111c29a40439018d870eb22",
        strip_prefix = "tempfile-3.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.tempfile-3.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__textwrap__0_11_0",
        url = "https://crates.io/api/v1/crates/textwrap/0.11.0/download",
        type = "tar.gz",
        strip_prefix = "textwrap-0.11.0",
        build_file = Label("//third_party/cargo/remote:BUILD.textwrap-0.11.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__tink_core__0_2_0",
        url = "https://crates.io/api/v1/crates/tink-core/0.2.0/download",
        type = "tar.gz",
        sha256 = "b7fefcbc7d04471b83ca8b3c445c376d45a8ab8fc35c3aa215df51f90aa95737",
        strip_prefix = "tink-core-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.tink-core-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__tink_proto__0_2_0",
        url = "https://crates.io/api/v1/crates/tink-proto/0.2.0/download",
        type = "tar.gz",
        sha256 = "6f097af9d2db1a08ea01008f88bc803d8bda25c13bfdc076df7449da907e259f",
        strip_prefix = "tink-proto-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.tink-proto-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__tink_signature__0_2_0",
        url = "https://crates.io/api/v1/crates/tink-signature/0.2.0/download",
        type = "tar.gz",
        sha256 = "c5b54df96bd2a0524d2ffb9f2b6aa9574a4c9a4834e7d8585907f5b04fcd0743",
        strip_prefix = "tink-signature-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.tink-signature-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__typed_arena__2_0_1",
        url = "https://crates.io/api/v1/crates/typed-arena/2.0.1/download",
        type = "tar.gz",
        sha256 = "0685c84d5d54d1c26f7d3eb96cd41550adb97baed141a761cf335d3d33bcd0ae",
        strip_prefix = "typed-arena-2.0.1",
        build_file = Label("//third_party/cargo/remote:BUILD.typed-arena-2.0.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__typenum__1_13_0",
        url = "https://crates.io/api/v1/crates/typenum/1.13.0/download",
        type = "tar.gz",
        sha256 = "879f6906492a7cd215bfa4cf595b600146ccfac0c79bcbd1f3000162af5e8b06",
        strip_prefix = "typenum-1.13.0",
        build_file = Label("//third_party/cargo/remote:BUILD.typenum-1.13.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__unicode_segmentation__1_7_1",
        url = "https://crates.io/api/v1/crates/unicode-segmentation/1.7.1/download",
        type = "tar.gz",
        sha256 = "bb0d2e7be6ae3a5fa87eed5fb451aff96f2573d2694942e40543ae0bbe19c796",
        strip_prefix = "unicode-segmentation-1.7.1",
        build_file = Label("//third_party/cargo/remote:BUILD.unicode-segmentation-1.7.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__unicode_width__0_1_9",
        url = "https://crates.io/api/v1/crates/unicode-width/0.1.9/download",
        type = "tar.gz",
        strip_prefix = "unicode-width-0.1.9",
        build_file = Label("//third_party/cargo/remote:BUILD.unicode-width-0.1.9.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__unicode_xid__0_2_1",
        url = "https://crates.io/api/v1/crates/unicode-xid/0.2.1/download",
        type = "tar.gz",
        sha256 = "f7fe0bb3479651439c9112f72b6c505038574c9fbb575ed1bf3b797fa39dd564",
        strip_prefix = "unicode-xid-0.2.1",
        build_file = Label("//third_party/cargo/remote:BUILD.unicode-xid-0.2.1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__uuid__0_8_2",
        url = "https://crates.io/api/v1/crates/uuid/0.8.2/download",
        type = "tar.gz",
        sha256 = "bc5cf98d8186244414c848017f0e2676b3fcb46807f6668a97dfe67359a3c4b7",
        strip_prefix = "uuid-0.8.2",
        build_file = Label("//third_party/cargo/remote:BUILD.uuid-0.8.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__vec_map__0_8_2",
        url = "https://crates.io/api/v1/crates/vec_map/0.8.2/download",
        type = "tar.gz",
        strip_prefix = "vec_map-0.8.2",
        build_file = Label("//third_party/cargo/remote:BUILD.vec_map-0.8.2.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__version_check__0_9_3",
        url = "https://crates.io/api/v1/crates/version_check/0.9.3/download",
        type = "tar.gz",
        sha256 = "5fecdca9a5291cc2b8dcf7dc02453fee791a280f3743cb0905f8822ae463b3fe",
        strip_prefix = "version_check-0.9.3",
        build_file = Label("//third_party/cargo/remote:BUILD.version_check-0.9.3.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__wasi__0_10_2_wasi_snapshot_preview1",
        url = "https://crates.io/api/v1/crates/wasi/0.10.2+wasi-snapshot-preview1/download",
        type = "tar.gz",
        sha256 = "fd6fbd9a79829dd1ad0cc20627bf1ed606756a7f77edff7b66b7064f9cb327c6",
        strip_prefix = "wasi-0.10.2+wasi-snapshot-preview1",
        build_file = Label("//third_party/cargo/remote:BUILD.wasi-0.10.2+wasi-snapshot-preview1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__wasi__0_9_0_wasi_snapshot_preview1",
        url = "https://crates.io/api/v1/crates/wasi/0.9.0+wasi-snapshot-preview1/download",
        type = "tar.gz",
        sha256 = "cccddf32554fecc6acb585f82a32a72e28b48f8c4c1883ddfeeeaa96f7d8e519",
        strip_prefix = "wasi-0.9.0+wasi-snapshot-preview1",
        build_file = Label("//third_party/cargo/remote:BUILD.wasi-0.9.0+wasi-snapshot-preview1.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__which__4_1_0",
        url = "https://crates.io/api/v1/crates/which/4.1.0/download",
        type = "tar.gz",
        sha256 = "b55551e42cbdf2ce2bedd2203d0cc08dba002c27510f86dab6d0ce304cba3dfe",
        strip_prefix = "which-4.1.0",
        build_file = Label("//third_party/cargo/remote:BUILD.which-4.1.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__winapi__0_3_9",
        url = "https://crates.io/api/v1/crates/winapi/0.3.9/download",
        type = "tar.gz",
        sha256 = "5c839a674fcd7a98952e593242ea400abe93992746761e38641405d28b00f419",
        strip_prefix = "winapi-0.3.9",
        build_file = Label("//third_party/cargo/remote:BUILD.winapi-0.3.9.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__winapi_i686_pc_windows_gnu__0_4_0",
        url = "https://crates.io/api/v1/crates/winapi-i686-pc-windows-gnu/0.4.0/download",
        type = "tar.gz",
        sha256 = "ac3b87c63620426dd9b991e5ce0329eff545bccbbb34f3be09ff6fb6ab51b7b6",
        strip_prefix = "winapi-i686-pc-windows-gnu-0.4.0",
        build_file = Label("//third_party/cargo/remote:BUILD.winapi-i686-pc-windows-gnu-0.4.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__winapi_x86_64_pc_windows_gnu__0_4_0",
        url = "https://crates.io/api/v1/crates/winapi-x86_64-pc-windows-gnu/0.4.0/download",
        type = "tar.gz",
        sha256 = "712e227841d057c1ee1cd2fb22fa7e5a5461ae8e48fa2ca79ec42cfc1931183f",
        strip_prefix = "winapi-x86_64-pc-windows-gnu-0.4.0",
        build_file = Label("//third_party/cargo/remote:BUILD.winapi-x86_64-pc-windows-gnu-0.4.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__wyz__0_2_0",
        url = "https://crates.io/api/v1/crates/wyz/0.2.0/download",
        type = "tar.gz",
        sha256 = "85e60b0d1b5f99db2556934e21937020776a5d31520bf169e851ac44e6420214",
        strip_prefix = "wyz-0.2.0",
        build_file = Label("//third_party/cargo/remote:BUILD.wyz-0.2.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__zeroize__1_3_0",
        url = "https://crates.io/api/v1/crates/zeroize/1.3.0/download",
        type = "tar.gz",
        sha256 = "4756f7db3f7b5574938c3eb1c117038b8e07f95ee6718c0efad4ac21508f1efd",
        strip_prefix = "zeroize-1.3.0",
        build_file = Label("//third_party/cargo/remote:BUILD.zeroize-1.3.0.bazel"),
    )

    maybe(
        http_archive,
        name = "raze__zeroize_derive__1_1_0",
        url = "https://crates.io/api/v1/crates/zeroize_derive/1.1.0/download",
        type = "tar.gz",
        sha256 = "a2c1e130bebaeab2f23886bf9acbaca14b092408c452543c857f66399cd6dab1",
        strip_prefix = "zeroize_derive-1.1.0",
        build_file = Label("//third_party/cargo/remote:BUILD.zeroize_derive-1.1.0.bazel"),
    )
