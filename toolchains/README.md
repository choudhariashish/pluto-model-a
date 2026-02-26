# Cross-Compilation Toolchains

This directory contains cross-compilation toolchains organized by target architecture and platform.

## Directory Structure

```
toolchains/
├── BUILD.bazel              # Platform definitions and config settings
├── arm64-linux-gnu/         # ARM64 Linux toolchains
│   └── bbai64/              # BeagleBone AI-64 specific toolchain
│       ├── BUILD.bazel      # Toolchain registration
│       ├── cc_toolchain_config.bzl  # Toolchain configuration
│       └── toolchain.BUILD  # External toolchain BUILD file
└── (future: riscv64-linux-gnu/, armv7-linux-gnueabihf/, etc.)
```

## Available Toolchains

### BeagleBone AI-64 (ARM64)

**Location:** `toolchains/arm64-linux-gnu/bbai64/`

**Target Platform:** BeagleBone AI-64 running Debian Bookworm

**Toolchain:** ARM GNU Toolchain 12.3.rel1
- **Architecture:** aarch64 (ARM64)
- **glibc version:** 2.36 (compatible with Debian Bookworm)
- **C++ Standard Library:** Statically linked libstdc++ to avoid version conflicts

**Usage:**
```bash
# Build for BeagleBone AI-64
bazel build --config=arm64 //your/target

# Build optimized release for BeagleBone AI-64
bazel build --config=release-arm64 //your/target

# Deploy to BeagleBone AI-64
bazel run //tools:deploy -- //your/target root@192.168.55.102
```

**Platform Targets:**
- `//toolchains:linux_aarch64` - Generic ARM64 Linux platform
- `//toolchains:bbai64` - BeagleBone AI-64 specific platform

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
| bbai64 | BeagleBone AI-64 | Debian Bookworm | 2.36 | ✅ |

## Notes

- All toolchains use absolute paths to avoid Bazel sandbox issues
- Static linking of libstdc++ is recommended to avoid version conflicts
- Toolchains are downloaded and cached by Bazel automatically
