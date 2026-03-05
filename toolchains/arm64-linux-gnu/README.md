# ARM64 Linux GNU Toolchains

This directory contains cross-compilation toolchains for ARM64 (aarch64) Linux targets using the GNU toolchain.

## Available Toolchains

### orin-nano - NVIDIA Orin Nano

**Target Device:** NVIDIA Orin Nano

**Operating System:** Ubuntu 22.04 LTS

**Toolchain Details:**
- **Name:** ARM GNU Toolchain 11.3.rel1
- **Triple:** aarch64-none-linux-gnu
- **GCC Version:** 11.3.1
- **glibc:** 2.35
- **Download:** ARM Developer website

**Why this version?**
- Ubuntu 22.04 LTS uses glibc 2.35
- ARM GNU Toolchain 11.3 is compatible with Ubuntu 22.04
- Provides stable long-term support

**Special Configuration:**
- Absolute paths to toolchain binaries for Bazel sandbox compatibility
- Optimized for NVIDIA Jetson platform

## Adding More ARM64 Targets

When adding new ARM64 Linux targets (e.g., Raspberry Pi, other Jetson devices, etc.):

1. Create a new subdirectory: `toolchains/arm64-linux-gnu/<device-name>/`
2. Consider the target's glibc version for toolchain selection
3. Use the orin-nano toolchain as a template
4. Update device-specific settings (e.g., CPU features, optimization flags)

## Toolchain Selection Guide

| Target OS | glibc Version | Recommended Toolchain |
|-----------|---------------|----------------------|
| Ubuntu 22.04 | 2.35 | ARM GNU 11.3 |
| Ubuntu 24.04 | 2.39 | ARM GNU 13.x |
| Debian Bookworm | 2.36 | ARM GNU 12.3 |
| Debian Trixie | 2.38+ | ARM GNU 13.x |

Check your target's glibc version:
```bash
ssh user@device "ldd --version"
```
