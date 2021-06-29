licenses(["notice"])

cc_binary(
    name = "cpp_interface",
    srcs = ["cpp_interface.cc"],
    copts = [
        "-Iexternal/souffle/src/include/souffle",
        "-std=c++17",
    ],
    deps = ["@souffle//:souffle_lib"],
)
