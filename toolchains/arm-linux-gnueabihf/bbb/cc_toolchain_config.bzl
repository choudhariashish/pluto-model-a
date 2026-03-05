load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "feature",
    "flag_group",
    "flag_set",
    "tool_path",
)

all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]

def _impl(ctx):
    # Use absolute paths to the downloaded toolchain in Bazel cache
    toolchain_bin = "/home/ashish/.cache/bazel/_bazel_ashish/7586bff8c7713718b6357f3d9a165b16/external/+_repo_rules2+bbb_arm_gnu_toolchain/bin"
    
    tool_paths = [
        tool_path(
            name = "gcc",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-gcc",
        ),
        tool_path(
            name = "g++",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-g++",
        ),
        tool_path(
            name = "ld",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-ld",
        ),
        tool_path(
            name = "ar",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-ar",
        ),
        tool_path(
            name = "cpp",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-cpp",
        ),
        tool_path(
            name = "gcov",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-gcov",
        ),
        tool_path(
            name = "nm",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-nm",
        ),
        tool_path(
            name = "objdump",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-objdump",
        ),
        tool_path(
            name = "strip",
            path = toolchain_bin + "/arm-none-linux-gnueabihf-strip",
        ),
    ]

    default_compiler_flags = feature(
        name = "default_compiler_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.assemble,
                    ACTION_NAMES.preprocess_assemble,
                    ACTION_NAMES.linkstamp_compile,
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_module_codegen,
                    ACTION_NAMES.lto_backend,
                    ACTION_NAMES.clif_match,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-march=armv7-a",
                            "-mfpu=neon",
                            "-mfloat-abi=hard",
                            "-O2",
                            "-Wall",
                            "-std=c++17",
                        ],
                    ),
                ],
            ),
        ],
    )

    default_linker_flags = feature(
        name = "default_linker_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-static",
                            "-lstdc++",
                            "-lm",
                            "-lpthread",
                        ],
                    ),
                ],
            ),
        ],
    )

    features = [
        default_compiler_flags,
        default_linker_flags,
    ]

    # Include directories from the downloaded toolchain
    toolchain_root = "/home/ashish/.cache/bazel/_bazel_ashish/7586bff8c7713718b6357f3d9a165b16/external/+_repo_rules2+bbb_arm_gnu_toolchain"
    
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = features,
        cxx_builtin_include_directories = [
            toolchain_root + "/arm-none-linux-gnueabihf/include/c++/13.3.1",
            toolchain_root + "/arm-none-linux-gnueabihf/include/c++/13.3.1/arm-none-linux-gnueabihf",
            toolchain_root + "/arm-none-linux-gnueabihf/libc/usr/include",
            toolchain_root + "/lib/gcc/arm-none-linux-gnueabihf/13.3.1/include",
            toolchain_root + "/lib/gcc/arm-none-linux-gnueabihf/13.3.1/include-fixed",
        ],
        toolchain_identifier = "bbb-armv7-toolchain",
        host_system_name = "local",
        target_system_name = "arm-none-linux-gnueabihf",
        target_cpu = "armv7",
        target_libc = "glibc_2.13",
        compiler = "gcc",
        abi_version = "eabi",
        abi_libc_version = "glibc_2.13",
        tool_paths = tool_paths,
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)
