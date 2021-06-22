"""Re-exports some "native" build rules.

Use these instead of using the external repos directly to make it
easier to import our code into internal Google repositories.
"""

load("@rules_proto//proto:defs.bzl", _proto_library = "proto_library")
load("@rules_proto_grpc//cpp:defs.bzl", _cc_proto_library = "cpp_proto_library")

cc_proto_library = _cc_proto_library
proto_library = _proto_library
