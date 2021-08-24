package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "ir",
    srcs = glob(
        ["*.cc"],
        exclude = ["*_test.cc"],
    ),
    hdrs = glob(["*.h"]),
    deps = [
        "@absl//absl/container:flat_hash_set",
        "@absl//absl/hash",
        "@absl//absl/strings",
        "@absl//absl/types:variant",
    ],
)

cc_test(
    name = "access_path_selectors_test",
    srcs = ["access_path_selectors_test.cc"],
    deps = [
        ":ir",
        "//src/common/testing:gtest",
        "@absl//absl/hash",
        "@absl//absl/hash:hash_testing",
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "access_path_selectors_set_test",
    srcs = ["access_path_selectors_set_test.cc"],
    deps = [
        ":ir",
        "//src/common/testing:gtest",
        "@absl//absl/container:btree",
        "@absl//absl/container:flat_hash_set",
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
