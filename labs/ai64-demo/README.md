# BeagleBone AI-64 Cross-Compilation Demo

Cross-compilation demo for BeagleBone AI-64 using Bazel with automatically downloaded ARM64 toolchain.

## Building

### Build for host (x86_64)
```bash
# Debug build
bazel build //labs/ai64-demo:hello
# or with config
bazel build --config=host //labs/ai64-demo:hello

# Release build (optimized)
bazel build --config=release-host //labs/ai64-demo:hello
```

### Build for ARM64 (BeagleBone AI-64)
```bash
# Debug build
bazel build --config=arm64 //labs/ai64-demo:hello

# Release build (optimized + stripped)
bazel build --config=release-arm64 //labs/ai64-demo:hello
```

## Running

### On host
```bash
./bazel-bin/labs/ai64-demo:hello
# Output: Hello from BeagleBone AI-64!
```

### Verify ARM64 binary
```bash
file bazel-bin/labs/ai64-demo/hello
# Output: ELF 64-bit LSB executable, ARM aarch64...
```

### Deploy to BeagleBone AI-64

#### Option 1: Using the deploy script (recommended)
```bash
# Deploy this specific target
bazel run //labs/ai64-demo:deploy

# Or use the universal deploy tool
bazel run //tools:deploy -- //labs/ai64-demo:hello

# Deploy to a different host
bazel run //tools:deploy -- //labs/ai64-demo:hello debian@192.168.1.100
```

#### Option 2: Manual deployment
1. Build and copy the binary to the board:
   ```bash
   bazel build --config=release-arm64 //labs/ai64-demo:hello
   scp bazel-bin/labs/ai64-demo/hello debian@beaglebone.local:~/
   ```

2. SSH into the board and run:
   ```bash
   ssh debian@beaglebone.local
   ./hello
   ```

## Toolchain Details

- **Host platform**: `//toolchains:linux_x86_64`
- **ARM64 platform**: `//toolchains:linux_aarch64`
- **Cross-compiler**: ARM GNU Toolchain 13.2.Rel1 (automatically downloaded by Bazel)
- **No system installation required**: Toolchain is managed entirely by Bazel

## How It Works

Bazel automatically:
1. Downloads the ARM64 cross-compiler toolchain (arm-gnu-toolchain-13.2.Rel1) on first build
2. Configures include paths and library search paths for the toolchain
3. Selects the appropriate compiler based on the `--platforms` flag
4. Uses absolute paths to the downloaded toolchain in Bazel's cache

The toolchain configuration uses absolute paths to the Bazel cache directory, which allows the cross-compiler to work without system installation.
