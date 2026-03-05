# BeagleBone Black Drive Demo

Motor drive control application for BeagleBone Black.

## Building

### Cross-Compilation (on host with Bazel)

```bash
# Build for BeagleBone Black
bazel build --config=bbb //labs/bbb-drive-demo:bbb-drive-demo

# Binary will be at:
# bazel-bin/labs/bbb-drive-demo/bbb-drive-demo
```

### Deployment

```bash
# Copy to BeagleBone Black
scp bazel-bin/labs/bbb-drive-demo/bbb-drive-demo root@192.168.55.102:/usr/local/bin/

# SSH and run
ssh root@192.168.55.102
bbb-drive-demo help
```

## Usage

```bash
bbb-drive-demo <command> [options]
```

## Technical Details

- **Toolchain:** ARM GNU Toolchain 13.3.1 (ARMv7 hard-float)
- **Target:** BeagleBone Black (ARMv7-A, NEON, hard-float ABI)
- **Linking:** Static (to avoid glibc version dependencies)

## Files

- `main.cpp` - Main application entry point
- `BUILD.bazel` - Bazel build configuration
- `README.md` - This file
