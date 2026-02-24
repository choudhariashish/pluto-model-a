bazel run //labs/pluto-network-demo:pnode-server -- -c etc/pluto-network-demo/pnode-config.txt &

bazel run //labs/pluto-network-demo/python:pnode-client -- -c  etc/pluto-network-demo/pnode-config.txt &

bazel run //labs/pluto-network-demo:pnode-cli -- -c etc/pluto-network-demo/pnode-config.txt 4 &
