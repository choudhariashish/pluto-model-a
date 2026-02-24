#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>
#include <iostream>
#include <fstream>
#include <string>

#include "PObject.h"
#include "PUtils.h"

#define NODE_NAME              "pnode-server"

int main(int argc, char** argv)
{
    // Parse command-line arguments
    std::string configPath = "pnode-config.txt";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            configPath = argv[i + 1];
            i++; // Skip next argument since we consumed it
        }
    }

    uint64_t runNetworkForSec = 100 * 20;
    int periodMs = 10;
    PObject::setupTimeoutControl(periodMs);

    AppSettings appSettings("AppSettings");
    appSettings.getSettings()->file = configPath.c_str();
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

    while (PObject::getTickMs() < runNetworkForSec) {}
    node.shutdown();
    runThread.join();

    return 0;
}
