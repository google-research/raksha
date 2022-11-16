# A Raksha parser for SQL using ZetaSQL

cc_test(
    name = "sql_parser_test",
    srcs = ["sql_parser_test.cc"],
    deps = [
        "//src/common/testing:gtest",
        "@com_google_absl//absl/strings",
        "@com_google_zetasql//zetasql/public:parse_helpers",
    ],
)

cc_library(
    name = "zeta_to_raksha",
    srcs = ["zeta_to_raksha.h"],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_google_zetasql//zetasql/public:parse_helpers",
        "@com_google_zetasql//zetasql/public:type",
        "@com_google_zetasql//zetasql/public:analyzer"
    ],
)

cc_test(
    name = "zeta_to_raksha_test",
    srcs = ["zeta_to_raksha_test.cc"],
    deps = [
        ":zeta_to_raksha",
        "//src/common/testing:gtest",
    ]
)