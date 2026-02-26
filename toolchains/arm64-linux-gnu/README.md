# ARM64 Linux GNU Toolchains

This directory contains cross-compilation toolchains for ARM64 (aarch64) Linux targets using the GNU toolchain.

## Available Toolchains

### bbai64 - BeagleBone AI-64

**Target Device:** BeagleBone AI-64

**Operating System:** Debian Bookworm (Debian 12)

**Toolchain Details:**
- **Name:** ARM GNU Toolchain 12.3.rel1
- **Triple:** aarch64-none-linux-gnu
- **GCC Version:** 12.3.1
- **glibc:** 2.36
- **Download:** ARM Developer website

**Why this version?**
- Debian Bookworm uses glibc 2.36
- ARM GNU Toolchain 13.x requires glibc 2.38 (incompatible)
- Version 12.3 is the latest compatible with Debian Bookworm

**Special Configuration:**
- Static linking of libstdc++ to avoid `GLIBCXX` version conflicts
- Absolute paths to toolchain binaries for Bazel sandbox compatibility

## Adding More ARM64 Targets

When adding new ARM64 Linux targets (e.g., Raspberry Pi, NVIDIA Jetson, etc.):

1. Create a new subdirectory: `toolchains/arm64-linux-gnu/<device-name>/`
2. Consider the target's glibc version for toolchain selection
3. Use the bbai64 toolchain as a template
4. Update device-specific settings (e.g., CPU features, optimization flags)

## Toolchain Selection Guide

| Target OS | glibc Version | Recommended Toolchain |
|-----------|---------------|----------------------|
| Debian Bookworm | 2.36 | ARM GNU 12.3 |
| Debian Trixie | 2.38+ | ARM GNU 13.x |
| Ubuntu 22.04 | 2.35 | ARM GNU 12.2 |
| Ubuntu 24.04 | 2.39 | ARM GNU 13.x |

Check your target's glibc version:
```bash
ssh user@device "ldd --version"
```
