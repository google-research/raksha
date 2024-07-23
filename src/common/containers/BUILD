# Containers for use in Raksha.

cc_library(
    name = "indexed_hash_map",
    hdrs = ["indexed_hash_map.h"],
    visibility = ["//src:__subpackages__"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:check",
    ],
)

cc_test(
    name = "indexed_hash_map_test",
    srcs = ["indexed_hash_map_test.cc"],
    deps = [
        ":indexed_hash_map",
        "//src/common/testing:gtest",
        "@fuzztest//fuzztest",
    ],
)
