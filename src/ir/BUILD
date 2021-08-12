package(default_visibility = ["//src/transform:__subpackages__"])

cc_library(
    name = "ir",
    srcs = ["access_path.cc"],
    hdrs = ["access_path.h"],
    linkopts = select({
        "@bazel_tools//src/conditions:linux_x86_64": [
            "-ldl",
        ],
        "//conditions:default": [
        ],
    }),
    deps = [
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
    ],
)

cc_test(
    name = "access_path_test",
    srcs = ["access_path_test.cc"],
    deps = [
        ":ir",
        "//src/common/testing:gtest",
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
    ],
)
