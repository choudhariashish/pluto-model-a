"""NVIDIA Orin Nano ARM64 cross-compilation toolchain configuration for Bazel.

This toolchain uses ARM GNU Toolchain 11.3 which is compatible with
Ubuntu 22.04's glibc 2.35.
"""

load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl", "tool_path", "feature", "flag_group", "flag_set")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

def _impl(ctx):
    # Use absolute paths to the downloaded toolchain
    toolchain_bin = "/home/ashish/.cache/bazel/_bazel_ashish/7586bff8c7713718b6357f3d9a165b16/external/+_repo_rules2+orin_nano_arm_gnu_toolchain/bin"
    
    tool_paths = [
        tool_path(name = "gcc", path = toolchain_bin + "/aarch64-none-linux-gnu-gcc"),
        tool_path(name = "g++", path = toolchain_bin + "/aarch64-none-linux-gnu-g++"),
        tool_path(name = "ar", path = toolchain_bin + "/aarch64-none-linux-gnu-ar"),
        tool_path(name = "ld", path = toolchain_bin + "/aarch64-none-linux-gnu-ld"),
        tool_path(name = "cpp", path = toolchain_bin + "/aarch64-none-linux-gnu-cpp"),
        tool_path(name = "gcov", path = toolchain_bin + "/aarch64-none-linux-gnu-gcov"),
        tool_path(name = "nm", path = toolchain_bin + "/aarch64-none-linux-gnu-nm"),
        tool_path(name = "objdump", path = toolchain_bin + "/aarch64-none-linux-gnu-objdump"),
        tool_path(name = "strip", path = toolchain_bin + "/aarch64-none-linux-gnu-strip"),
    ]

    # Include directories from the downloaded toolchain
    toolchain_root = "/home/ashish/.cache/bazel/_bazel_ashish/7586bff8c7713718b6357f3d9a165b16/external/+_repo_rules2+orin_nano_arm_gnu_toolchain"
    sysroot = toolchain_root + "/aarch64-none-linux-gnu/libc"
    
    cxx_builtin_include_directories = [
        toolchain_root + "/aarch64-none-linux-gnu/include/c++/11.3.1",
        toolchain_root + "/aarch64-none-linux-gnu/include/c++/11.3.1/aarch64-none-linux-gnu",
        toolchain_root + "/aarch64-none-linux-gnu/libc/usr/include",
        toolchain_root + "/lib/gcc/aarch64-none-linux-gnu/11.3.1/include",
        toolchain_root + "/lib/gcc/aarch64-none-linux-gnu/11.3.1/include-fixed",
        sysroot + "/usr/include",
        sysroot + "/usr/include/aarch64-linux-gnu",
        sysroot + "/usr/include/linux",
        sysroot + "/usr/include/asm-generic",
    ]
    
    # Add compile flags for sysroot
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "--sysroot=" + sysroot,
                            "-I" + sysroot + "/usr/include/aarch64-linux-gnu",
                        ],
                    ),
                ],
            ),
        ],
    )
    
    # Add default link flags
    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "--sysroot=" + sysroot,
                            "-B" + sysroot + "/usr/lib/aarch64-linux-gnu",
                            "-L" + toolchain_root + "/aarch64-none-linux-gnu/lib64",
                            "-L" + toolchain_root + "/lib/gcc/aarch64-none-linux-gnu/11.3.1",
                            "-L" + sysroot + "/usr/lib/aarch64-linux-gnu",
                            "-L" + sysroot + "/lib/aarch64-linux-gnu",
                        ],
                    ),
                ],
            ),
        ],
    )
    
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "orin-nano-arm64-toolchain",
        host_system_name = "local",
        target_system_name = "aarch64-linux-gnu",
        target_cpu = "aarch64",
        target_libc = "glibc",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        builtin_sysroot = sysroot,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        features = [default_compile_flags_feature, default_link_flags_feature],
    )

orin_nano_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)
