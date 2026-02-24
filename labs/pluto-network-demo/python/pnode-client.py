import sys
import argparse
from lib.PUtils.python.PUtils import *

NODE_NAME  = "pnode-client"

# Parse command-line arguments
parser = argparse.ArgumentParser(description='PNode Client')
parser.add_argument('-c', '--config', type=str, 
                    default='../../../etc/pluto-network-demo/pnode-config.src.txt',
                    help='Path to config file or directory containing pnode-config.txt')
args = parser.parse_args()

# Determine config file path
import os
if os.path.isdir(args.config):
    config_path = os.path.join(args.config, 'pnode-config.src.txt')
else:
    config_path = args.config

appSettings = AppSettings()
s = appSettings.load(config_path)

node = NetworkNode()
node.settings_ = s[NODE_NAME]
node.initialize()
thread = threading.Thread(target=node.run)
thread.start()

# msg = Message(dstId=2, mid=2, payload=b"Hello World")
# for i in range(0, 5):
#     if node.sendQuery(msg, 1000):
#         print("node.sendQuery responded")
#     else:
#         print("node.sendQuery not responded")

timeoutMs = 10000
while timeoutMs > 0:
    m = node.recv()
    if m != None:
        if m.rspNeeded:
            m.dstId = m.srcId
            node.respQuery(m)

    time.sleep(0.01)
    timeoutMs -= 10

node.shutdown()
time.sleep(1)

thread.join()