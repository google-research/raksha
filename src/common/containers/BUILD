# Containers for use in Raksha. Most of these are a thin layer around either an Abseil or a standard
# library container. While we usually want to use the more-performant Abseil containers, this is
# useful for performing conditional compilation to choose between the two when using Abseil is not
# desirable or available.

cc_library(
    name = "hash_map",
    hdrs = ["hash_map.h"],
    visibility = ["//src:__subpackages__"],
    deps = ["@com_google_absl//absl/container:flat_hash_map"],
)

cc_library(
    name = "hash_set",
    hdrs = ["hash_set.h"],
    visibility = ["//src:__subpackages__"],
    deps = ["@com_google_absl//absl/container:flat_hash_set"],
)
