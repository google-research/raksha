package(default_visibility = ["//src/transform:__subpackages__"])

cc_library(
    name = "ir",
    srcs = glob(
        ["*.cc"],
        exclude = ["*_test.cc"],
    ),
    hdrs = glob(["*.h"]),
    linkopts = select({
        "@bazel_tools//src/conditions:linux_x86_64": [
            "-ldl",
        ],
        "//conditions:default": [
        ],
    }),
    deps = [
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/hash",
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
        "@absl//absl/types:variant",
    ],
)

cc_test(
    name = "selector_access_path_test",
    srcs = ["selector_access_path_test.cc"],
    deps = [
        ":ir",
        "//src/common/testing:gtest",
        "@absl//absl/hash",
        "@absl//absl/hash:hash_testing",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "field_selector_test",
    srcs = ["field_selector_test.cc"],
    deps = [
        ":ir",
        "//src/common/testing:gtest",
        "@absl//absl/hash",
        "@absl//absl/hash:hash_testing",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "selector_test",
    srcs = ["selector_test.cc"],
    deps = [
        ":ir",
        "//src/common/testing:gtest",
        "@absl//absl/hash",
        "@absl//absl/hash:hash_testing",
        "@absl//absl/strings",
    ],
)
