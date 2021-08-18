package(default_visibility = ["//src:__subpackages__"])

cc_library(
    name = "util",
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
    deps = ["@absl//absl/strings"],
)

cc_test(
    name = "copyable_test",
    srcs = ["copyable_test.cc"],
    deps = [
        ":util",
        "//src/common/testing:gtest",
    ],
)
