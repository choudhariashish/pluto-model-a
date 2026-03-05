#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAIN_ROOT="$(dirname "$(dirname "$(dirname "$SCRIPT_DIR")")")"
exec "$TOOLCHAIN_ROOT/external/bbb_arm_gnu_toolchain/bin/arm-none-linux-gnueabihf-nm" "$@"
