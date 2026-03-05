# Cross-Compilation Toolchains

This directory contains cross-compilation toolchains organized by target architecture and platform.

## Directory Structure

```
toolchains/
├── BUILD.bazel              # Platform definitions and config settings
├── arm64-linux-gnu/         # ARM64 Linux toolchains
│   └── orin-nano/           # NVIDIA Orin Nano specific toolchain
│       ├── BUILD.bazel      # Toolchain registration
│       ├── cc_toolchain_config.bzl  # Toolchain configuration
│       └── toolchain.BUILD  # External toolchain BUILD file
├── arm-linux-gnueabihf/     # ARMv7 Linux toolchains
│   └── bbb/                 # BeagleBone Black specific toolchain
└── (future: riscv64-linux-gnu/, etc.)
```

## Available Toolchains

### NVIDIA Orin Nano (ARM64)

**Location:** `toolchains/arm64-linux-gnu/orin-nano/`

**Target Platform:** NVIDIA Orin Nano running Ubuntu 22.04

**Toolchain:** ARM GNU Toolchain 11.3.rel1
- **Architecture:** aarch64 (ARM64)
- **glibc version:** 2.35 (compatible with Ubuntu 22.04)

### BeagleBone Black (ARMv7)

**Location:** `toolchains/arm-linux-gnueabihf/bbb/`

**Target Platform:** BeagleBone Black running Debian 7

**Toolchain:** ARM GNU Toolchain 13.3.rel1
- **Architecture:** ARMv7 hard-float
- **glibc version:** 2.13+ compatible

**Config Settings:**
- `//toolchains:aarch64` - Detects ARM64 builds (use in `select()`)
- `//toolchains:x86_64` - Detects x86_64 builds (use in `select()`)

## Adding New Toolchains

To add a new cross-compilation toolchain:

1. **Create directory structure:**
   ```bash
   mkdir -p toolchains/<arch>-<os>-<abi>/<device-name>
   ```

2. **Add toolchain files:**
   - `BUILD.bazel` - Toolchain registration
   - `cc_toolchain_config.bzl` - Toolchain configuration
   - `toolchain.BUILD` - External toolchain BUILD file

3. **Register in MODULE.bazel:**
   ```python
   http_archive(
       name = "<device>_<arch>_toolchain",
       urls = ["<toolchain-url>"],
       strip_prefix = "<archive-prefix>",
       build_file = "//toolchains/<arch>-<os>-<abi>/<device>:toolchain.BUILD",
   )
   ```

4. **Register in .bazelrc:**
   ```
   build --extra_toolchains=//toolchains/<arch>-<os>-<abi>/<device>:<toolchain-name>
   ```

5. **Add platform definition in toolchains/BUILD.bazel:**
   ```python
   platform(
       name = "<device>",
       constraint_values = [
           "@platforms//os:<os>",
           "@platforms//cpu:<cpu>",
       ],
   )
   ```

## Example: Using Toolchain in BUILD Files

```python
cc_binary(
    name = "my_app",
    srcs = ["main.cpp"],
    linkopts = select({
        "//toolchains:aarch64": [
            # ARM64-specific linker options
            "-Wl,-Bstatic",
            "-lstdc++",
            "-Wl,-Bdynamic",
        ],
        "//conditions:default": [],
    }),
)
```

## Toolchain Compatibility

| Toolchain | Target Device | OS | glibc | Tested |
|-----------|--------------|-----|-------|--------|
| orin-nano | NVIDIA Orin Nano | Ubuntu 22.04 | 2.35 | ✅ |
| bbb | BeagleBone Black | Debian 7 | 2.13 | ✅ |

## Notes

- All toolchains use absolute paths to avoid Bazel sandbox issues
- Static linking of libstdc++ is recommended to avoid version conflicts
- Toolchains are downloaded and cached by Bazel automatically
