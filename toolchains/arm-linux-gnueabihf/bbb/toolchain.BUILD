package(default_visibility = ["//visibility:public"])

filegroup(
    name = "gcc",
    srcs = [
        "bin/arm-none-linux-gnueabihf-gcc",
    ],
)

filegroup(
    name = "ar",
    srcs = [
        "bin/arm-none-linux-gnueabihf-ar",
    ],
)

filegroup(
    name = "ld",
    srcs = [
        "bin/arm-none-linux-gnueabihf-ld",
    ],
)

filegroup(
    name = "nm",
    srcs = [
        "bin/arm-none-linux-gnueabihf-nm",
    ],
)

filegroup(
    name = "objcopy",
    srcs = [
        "bin/arm-none-linux-gnueabihf-objcopy",
    ],
)

filegroup(
    name = "objdump",
    srcs = [
        "bin/arm-none-linux-gnueabihf-objdump",
    ],
)

filegroup(
    name = "strip",
    srcs = [
        "bin/arm-none-linux-gnueabihf-strip",
    ],
)

filegroup(
    name = "as",
    srcs = [
        "bin/arm-none-linux-gnueabihf-as",
    ],
)

filegroup(
    name = "compiler_pieces",
    srcs = glob([
        "**",
    ]),
)

filegroup(
    name = "compiler_components",
    srcs = [
        ":gcc",
        ":ar",
        ":ld",
        ":nm",
        ":objcopy",
        ":objdump",
        ":strip",
        ":as",
    ],
)
