#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>
#include <iostream>
#include <fstream>
#include <string>

#include "PObject.h"
#include "PUtils.h"
#include "PMessage.h"

#define NODES_FILE             "etc/pnode-config.txt"
#define NODE_NAME              "pnode-cli"

int main(int argc, char** argv)
{
    uint64_t runNetworkForSec = 100 * 20;
    int periodMs = 10;
    PObject::setupTimeoutControl(periodMs);

    AppSettings appSettings("AppSettings");
    appSettings.getSettings()->file = NODES_FILE;
    appSettings.initialize();
    NetworkNode node(NODE_NAME);
    auto s = node.getSettings();

    s->serverIp             = appSettings.getData()->doc[NODE_NAME]["serverIp"].GetString();
    s->port                 = appSettings.getData()->doc[NODE_NAME]["port"].GetInt();
    s->nodeType             = (strcmp(appSettings.getData()->doc[NODE_NAME]["nodeType"].GetString(), "client")==0) ? NetworkNode::NodeType_t::CLIENT_NODE : NetworkNode::NodeType_t::SERVER_NODE;
    s->nodeId               = appSettings.getData()->doc[NODE_NAME]["nodeId"].GetInt();
    s->maxNodeIds           = appSettings.getData()->doc[NODE_NAME]["maxNodeIds"].GetInt();
    s->keepAliveUntilTimeMs = appSettings.getData()->doc[NODE_NAME]["keepAliveUntilTimeMs"].GetInt();
    s->maxNewMsgSize        = appSettings.getData()->doc[NODE_NAME]["maxNewMsgSize"].GetInt();
    s->maxRspMsgSize        = appSettings.getData()->doc[NODE_NAME]["maxRspMsgSize"].GetInt();
    s->logTransitMsgs       = appSettings.getData()->doc[NODE_NAME]["logTransitMsgs"].GetBool();

    node.initialize();
    std::thread runThread(&NetworkNode::run, &node);

    uint64_t t0 = PObject::getTickMs() + 10;
    while (PObject::getTickMs() < runNetworkForSec)
    {
        NetworkNode::Message_t m = {0};

        if (PObject::getTickMs() > t0)
        {
            t0 = PObject::getTickMs() + 100;

            // node.sendQuery(atoi(argv[3]), MessageId::GET_NODES, "node.sendQuery", &m, 1000);
            // node.sendQuery(0, MessageId::GET_NODES, "node.sendQuery", &m, 1000);

            // node.send(atoi(argv[3], MessageId::INFO, "node.send");
            node.sendQuery(atoi(argv[3]), MessageId::INFO, "node.sendQuery", &m, 1000);
        }

        if (NetworkNode::Status_t::PL_OK == node.recv(&m))
        {
            if (m.rspNeeded)
            {
                node.respQuery(m.srcId, m.mid, "resp msg", m.seq);
            }
        }
    }
    node.shutdown();
    runThread.join();
    return 0;
}
