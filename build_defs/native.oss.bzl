"""Re-exports some "native" build rules.

Use these instead of using the external repos directly to make it
easier to import our code into internal Google repositories.
"""

load("@rules_proto//proto:defs.bzl", _proto_library = "proto_library")
load("@rules_proto_grpc//cpp:defs.bzl", _cc_proto_library = "cpp_proto_library")

cc_proto_library = _cc_proto_library
proto_library = _proto_library

def raksha_cc_proto_library(
        name,
        deps = [],
        protos = [],
        visibility = None):
    """Used to paper over differences in interface between external and internal versions of cc_proto_library.

    Args:
        name: String; Name of the library
        deps: List; labels of protos used to produce this library
        protos: List; Same as deps, will be removed shortly.
        visibility: List; List of visibilities
    """
    _cc_proto_library(
        name = name,
        protos = deps + protos,
        visibility = visibility,
    )
