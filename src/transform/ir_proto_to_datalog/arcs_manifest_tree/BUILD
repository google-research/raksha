cc_library(
    name = "arcs_manifest_tree_lib",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    linkopts = select({
        "@bazel_tools//src/conditions:linux_x86_64": [
            "-ldl",
        ],
        "//conditions:default": [
        ],
    }),
    deps = [
        "//src/ir",
        "//src/transform/ir_proto_to_datalog/logging:logging_lib",
        "//third_party/arcs/proto:manifest_cc_proto",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
        "@absl//absl/types:optional",
        "@absl//absl/types:variant",
    ],
)
