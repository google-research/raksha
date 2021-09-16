package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "types",
    srcs = glob(
        ["*.cc"],
        exclude = ["*_test.cc"],
    ),
    hdrs = glob(["*.h"]),
    deps = [
        "//src/common/logging",
        "//src/ir",
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
        ":types",
        "//src/common/testing:gtest",
        "//src/ir",
    ],
)

cc_test(
    name = "type_test",
    srcs = ["type_test.cc"],
    deps = [
        ":types",
        "//src/common/logging",
        "//src/common/testing:gtest",
        "//src/ir",
        "//src/ir/proto:types",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/strings",
    ],
)
