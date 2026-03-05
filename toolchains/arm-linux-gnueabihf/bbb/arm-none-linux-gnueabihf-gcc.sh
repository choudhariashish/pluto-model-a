#!/bin/bash
set -euo pipefail

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# The toolchain is available via runfiles
# Bazel makes external repositories available in the runfiles tree
TOOLCHAIN_BIN="external/bbb_arm_gnu_toolchain/bin/arm-none-linux-gnueabihf-gcc"

# Try to find it relative to the execroot
if [[ -n "${RUNFILES_DIR:-}" ]]; then
    TOOLCHAIN_PATH="$RUNFILES_DIR/$TOOLCHAIN_BIN"
elif [[ -f "$SCRIPT_DIR/../../../external/bbb_arm_gnu_toolchain/bin/arm-none-linux-gnueabihf-gcc" ]]; then
    TOOLCHAIN_PATH="$SCRIPT_DIR/../../../external/bbb_arm_gnu_toolchain/bin/arm-none-linux-gnueabihf-gcc"
else
    # Fallback: search for it in the execroot
    TOOLCHAIN_PATH="$(find . -path "*/external/bbb_arm_gnu_toolchain/bin/arm-none-linux-gnueabihf-gcc" 2>/dev/null | head -1)"
fi

if [[ ! -f "$TOOLCHAIN_PATH" ]]; then
    echo "Error: Could not find arm-none-linux-gnueabihf-gcc" >&2
    echo "Searched: $TOOLCHAIN_PATH" >&2
    exit 1
fi

exec "$TOOLCHAIN_PATH" "$@"
