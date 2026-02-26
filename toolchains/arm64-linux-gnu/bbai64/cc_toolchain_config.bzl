"""BeagleBone AI-64 ARM64 cross-compilation toolchain configuration for Bazel.

This toolchain uses ARM GNU Toolchain 12.3 which is compatible with
Debian Bookworm's glibc 2.36.
"""

load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl", "tool_path", "feature", "flag_group", "flag_set")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

def _impl(ctx):
    # Use absolute paths to the downloaded toolchain
    # Bazel downloads external repos to: <output_base>/external/<repo_name>
    # The execroot symlinks to this location
    toolchain_bin = "/home/ashish/.cache/bazel/_bazel_ashish/7586bff8c7713718b6357f3d9a165b16/external/+_repo_rules2+bbai64_arm_gnu_toolchain/bin"
    
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
    toolchain_root = "/home/ashish/.cache/bazel/_bazel_ashish/7586bff8c7713718b6357f3d9a165b16/external/+_repo_rules2+bbai64_arm_gnu_toolchain"
    
    cxx_builtin_include_directories = [
        toolchain_root + "/aarch64-none-linux-gnu/include/c++/12.3.1",
        toolchain_root + "/aarch64-none-linux-gnu/include/c++/12.3.1/aarch64-none-linux-gnu",
        toolchain_root + "/aarch64-none-linux-gnu/libc/usr/include",
        toolchain_root + "/lib/gcc/aarch64-none-linux-gnu/12.3.1/include",
        toolchain_root + "/lib/gcc/aarch64-none-linux-gnu/12.3.1/include-fixed",
    ]
    
    # Add default link flags to find libraries
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
                            "-L" + toolchain_root + "/aarch64-none-linux-gnu/lib64",
                            "-L" + toolchain_root + "/lib/gcc/aarch64-none-linux-gnu/12.3.1",
                        ],
                    ),
                ],
            ),
        ],
    )
    
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "bbai64-arm64-toolchain",
        host_system_name = "local",
        target_system_name = "aarch64-linux-gnu",
        target_cpu = "aarch64",
        target_libc = "glibc",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        features = [default_link_flags_feature],
    )

bbai64_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)
