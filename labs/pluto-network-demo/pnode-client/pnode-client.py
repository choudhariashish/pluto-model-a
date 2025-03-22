import sys
sys.path.append('../../../lib/PUtils/python/')

from PUtils import *

NODES_FILE = "../pnode.config"
NODE_NAME  = "pnode-client"

appSettings = AppSettings()
s = appSettings.load(NODES_FILE)

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