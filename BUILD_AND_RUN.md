# Build and Run Guide

Build and run pluto-network-demo targets using Bazel 8.1.1 with dependencies managed in a Python venv.

## Prerequisites

- Bazel 8.1.1 (`.bazelversion`)
- Python 3.12 (`MODULE.bazel`)

## Setup

Activate the Python virtual environment:

```bash
source venv/bin/activate
```

## Build Targets

```bash
# Python client
bazel build //labs/pluto-network-demo/python:pnode-client

# C++ server
bazel build //labs/pluto-network-demo:pnode-server

# C++ client
bazel build //labs/pluto-network-demo:pnode-cli
```

## Run Targets

All targets require the config file path:

```bash
# Python client
bazel run //labs/pluto-network-demo/python:pnode-client -- -c etc/pluto-network-demo/pnode-config.txt

# C++ server
bazel run //labs/pluto-network-demo:pnode-server -- -c etc/pluto-network-demo/pnode-config.txt

# C++ client (requires destination node ID)
bazel run //labs/pluto-network-demo:pnode-cli -- -c etc/pluto-network-demo/pnode-config.txt 4
```

## Key Files

- **`MODULE.bazel`** - Python toolchain and pip dependencies
- **`.bazelrc`** - Python isolation settings
- **`requirements.txt`** - Python dependencies with exact versions
- **`venv/`** - Python virtual environment

## Dependency Management

Bazel automatically fetches all Python dependencies from PyPI via the pip extension configured in `MODULE.bazel`. No manual installation needed - everything is isolated from the host system.
