# BUILD file for the BeagleBone AI-64 ARM64 cross-compiler toolchain
# ARM GNU Toolchain 12.3 (compatible with Debian Bookworm glibc 2.36)

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "all_files",
    srcs = glob(["**/*"], exclude = ["**/*.tar.xz"]),
)

filegroup(
    name = "compiler_files",
    srcs = glob([
        "bin/**",
        "aarch64-none-linux-gnu/**",
        "lib/**",
        "libexec/**",
        "include/**",
    ]),
)

filegroup(
    name = "linker_files",
    srcs = glob([
        "bin/**",
        "aarch64-none-linux-gnu/**",
        "lib/**",
        "libexec/**",
    ]),
)

filegroup(
    name = "ar_files",
    srcs = glob(["bin/**"]),
)
