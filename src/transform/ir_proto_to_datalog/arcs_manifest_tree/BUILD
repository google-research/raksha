cc_library(
    name = "arcs_manifest_tree_lib",
    srcs = glob(
        ["*.cc"],
        exclude = ["*_test.cc"],
    ),
    hdrs = glob(["*.h"]),
    deps = [
        "//src/ir",
        "//third_party/arcs/proto:manifest_cc_proto",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings",
        "@absl//absl/types:optional",
        "@absl//absl/types:variant",
    ],
)

cc_test(
    name = "primitive_type_test",
    srcs = ["primitive_type_test.cc"],
    deps = [
        ":arcs_manifest_tree_lib",
        "//src/common/testing:gtest",
        "//src/ir",
    ],
)

cc_test(
    name = "type_test",
    srcs = ["type_test.cc"],
    deps = [
        ":arcs_manifest_tree_lib",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "//src/ir",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/strings",
    ],
)
