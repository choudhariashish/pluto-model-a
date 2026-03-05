# BUILD file for ARM GNU Toolchain external repository
# This makes the toolchain files available to Bazel

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "all_files",
    srcs = glob(["**/*"]),
)

filegroup(
    name = "compiler_files",
    srcs = glob([
        "bin/aarch64-none-linux-gnu-gcc",
        "bin/aarch64-none-linux-gnu-g++",
        "bin/aarch64-none-linux-gnu-cpp",
        "lib/gcc/**",
        "libexec/**",
        "aarch64-none-linux-gnu/include/**",
    ]),
)

filegroup(
    name = "linker_files",
    srcs = glob([
        "bin/aarch64-none-linux-gnu-ld",
        "bin/aarch64-none-linux-gnu-gcc",
        "lib/gcc/**",
        "aarch64-none-linux-gnu/lib/**",
    ]),
)

filegroup(
    name = "ar_files",
    srcs = glob([
        "bin/aarch64-none-linux-gnu-ar",
    ]),
)
