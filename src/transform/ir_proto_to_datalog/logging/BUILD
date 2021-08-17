package(default_visibility = ["//src/transform:__subpackages__"])

cc_library(
    name = "logging_lib",
    srcs = glob(["*.cc"]),
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
