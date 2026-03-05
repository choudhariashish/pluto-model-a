# BeagleBone Black Hello World

Simple "Hello World" application demonstrating cross-compilation for BeagleBone Black using Bazel.

## Building

### Cross-Compilation (on host with Bazel)

```bash
# Build for BeagleBone Black
bazel build --config=bbb //labs/bbb-hello-world:bbb-hello-world

# Binary will be at:
# bazel-bin/labs/bbb-hello-world/bbb-hello-world
```

### Deployment

```bash
# Copy to BeagleBone Black
scp bazel-bin/labs/bbb-hello-world/bbb-hello-world root@192.168.55.102:/tmp/

# SSH and run
ssh root@192.168.55.102
/tmp/bbb-hello-world
```

## Output

```
Hello World
```

## Technical Details

- **Toolchain:** ARM GNU Toolchain 13.3.1 (ARMv7 hard-float)
- **Target:** BeagleBone Black (ARMv7-A, NEON, hard-float ABI)
- **Linking:** Static (to avoid glibc version dependencies)
- **Binary:** Statically linked, no external dependencies required

The binary is compiled with `-static` to ensure compatibility with older glibc versions (2.13+) on BeagleBone Black.

## Files

- `main.cpp` - Simple C++ program that prints "Hello World"
- `BUILD.bazel` - Bazel build configuration
- `README.md` - This file

## Toolchain Configuration

The cross-compiler is automatically downloaded and managed by Bazel. No installation required on the host system.

Configuration files:
- `MODULE.bazel` - Defines the ARM toolchain download
- `toolchains/arm-linux-gnueabihf/bbb/cc_toolchain_config.bzl` - Toolchain configuration
- `.bazelrc` - Build configuration for `--config=bbb`
