package(
    default_visibility = ["//src:__subpackages__"],
    features = ["layering_check"],
    licenses = ["notice"],
)

cc_test(
    name = "ambient_context_sql_test",
    srcs = ["AmbientContextSqlPolicyTest.cc"],
    copts = ["-fexceptions"],
    features = ["-use_header_modules"],
    deps = [
        "//src/common/testing:gtest",
        "//src/policy:parser",
    ],
)
