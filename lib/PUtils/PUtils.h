#ifndef LIB_PUTILS
#define LIB_PUTILS

#include "PObject.h"

#include <string>
#include <vector>
#include <map>
#include <enet/enet.h>
#include "rapidjson/document.h"

class UtilsObject : public PObject
{
public:
    UtilsObject(const char* name) : PObject(name) {}
private:
};


// Following message ids have to be used with NetworkNode framework.
enum MessageId
{
    INFO,
    DEBUG,
    WARN,
    ERROR,
    GET_NODES,    // Gets info on all the nodes in the network.
    MAX_NUM_MID
};


class NetworkNode : public UtilsObject
{
#define NETWORK_NODE_MSG_PAYLOAD_SIZE    1024
public:
    NetworkNode(const char* name);

    enum NodeType_t { SERVER_NODE, CLIENT_NODE };

    struct Message_t
    {
        int32_t mid;
        uint8_t seq;
        int32_t dstId;
        int32_t srcId;
        bool rspNeeded;
        bool isRspMsg;
        char payload[NETWORK_NODE_MSG_PAYLOAD_SIZE];
    };

    struct Settings_t
    {
        std::string serverIp;
        uint32_t port;
        enum NodeType_t nodeType;
        int32_t nodeId;
        uint8_t maxNodeIds;
        uint32_t keepAliveUntilTimeMs;
        uint32_t maxNewMsgSize;
        uint32_t maxRspMsgSize;
        bool logTransitMsgs;
    };
    struct Settings_t* getSettings(void) { return &settings_; }

    struct Data_t
    {
    };
    struct Data_t* getData(void) { return &data_; }

    Status_t initialize(void) override;
    Status_t run(void) override;
    Status_t shutdown(void) override;

    Status_t send(Message_t* msg, bool updateSeq=true);
    Status_t recv(Message_t* msg);

    Status_t send(int nodeId, int command, char* payload, int length);
    Status_t send(int nodeId, int command, const char* payload);

    Status_t sendQuery(int nodeId, int command, char* payload, int length, Message_t* rspMsg, uint64_t timeoutMs);
    Status_t sendQuery(int nodeId, int command, const char* payload, Message_t* rspMsg, uint64_t timeoutMs);

    Status_t respQuery(int nodeId, int command, char* payload, int length, uint8_t seq);
    Status_t respQuery(int nodeId, int command, const char* payload, uint8_t seq);

    Status_t recvQuery(Message_t* msg, uint64_t timeoutMs);

private:
    struct Settings_t settings_;
    struct Data_t data_;

    ENetAddress address_;
    ENetHost *host_;
    ENetPeer *peer_;
    ENetEvent event_;
    int eventStatus_;
    volatile bool shutdownCalled_;

    std::map<int, ENetPeer*> peerMap_;
    std::vector<Message_t> newMsg_;
    std::vector<Message_t> rspMsg_;
    uint8_t msgSeq_;
    enet_uint8 channel_;

    void handleServerMsg_(Message_t* m);
};

class AppSettings : public UtilsObject
{
public:
    AppSettings(const char* name);

    struct Settings_t
    {
        const char* file;
    };
    struct Settings_t* getSettings(void) { return &settings_; }

    struct Data_t
    {
        rapidjson::Document doc;
    };
    struct Data_t* getData(void) { return &data_; }

    Status_t initialize(void) override;
    Status_t run(void) override;
    Status_t shutdown(void) override;

private:
    struct Settings_t settings_;
    struct Data_t data_;
};

#endif // LIB_PUTILS
