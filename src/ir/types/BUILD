package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "types",
    srcs = ["schema.cc"],
    hdrs = [
        "entity_type.h",
        "primitive_type.h",
        "schema.h",
        "type.h",
    ],
    deps = [
        "//src/common/logging",
        "//src/ir:access_path",
        "@absl//absl/container:flat_hash_map",
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/strings",
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
