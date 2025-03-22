#include "PUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>  /* For htonl and ntohl */
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace rapidjson;

const char* MessageIdStr[] = {
    "INFO",
    "DEBUG",
    "WARN",
    "ERROR",
    "GET_NODES",
    "MAX_NUM_MID"
};


NetworkNode::NetworkNode(const char* name) : UtilsObject(name)
{
    enet_address_set_host(&address_, "localhost");
    host_ = NULL;
    peer_ = NULL;
    eventStatus_ = 0;
    shutdownCalled_ = false;
    settings_.maxNewMsgSize = 10;
    settings_.maxRspMsgSize = 10;
    settings_.logTransitMsgs = false;
    msgSeq_ = 0;
    channel_ = 0;
}

PObject::Status_t NetworkNode::initialize(void)
{
    if (enet_initialize() != 0)
    {
        printf("An error occurred while initializing ENet.\n");
        return Status_t::PL_NOT_OK;
    }

    address_.port = settings_.port;
    enet_address_set_host(&address_, settings_.serverIp.c_str());

    if (NetworkNode::NodeType_t::SERVER_NODE == settings_.nodeType)
    {
                                            // Max peers          Max channels
        host_ = enet_host_create(&address_, settings_.maxNodeIds, 1, 0, 0);
        if (host_ == NULL)
        {
            printf("An error occurred while trying to create an ENet server host.\n");
            return Status_t::PL_NOT_OK;
        }
        printf("SNode: %s:%u\n", settings_.serverIp.c_str(), settings_.port);
    }
    else    // CLIENT_NODE
    {
                                                             //   1kbps      1kbps
        host_ = enet_host_create(NULL, settings_.maxNodeIds, 1, 1024*1024, 1024*1024);
        if (host_ == NULL)
        {
            printf("An error occurred while trying to create an ENet server host.\n");
            return Status_t::PL_NOT_OK;
        }
        peer_ = enet_host_connect(host_, &address_, settings_.maxNodeIds, settings_.nodeId);
        if (peer_ == NULL)
        {
            printf("No available peers for initializing an ENet connection");
            return Status_t::PL_NOT_OK;
        }
        printf("CNode: %s:%u\n", settings_.serverIp.c_str(), settings_.port);
    }
    return Status_t::PL_OK;
}

PObject::Status_t NetworkNode::run(void)
{
    char buffer[128] = {0};
    Message_t m = {0};

    while (shutdownCalled_ == false)
    {
        if (NetworkNode::NodeType_t::SERVER_NODE == settings_.nodeType)
        {
            eventStatus_ = 1;
            eventStatus_ = enet_host_service(host_, &event_, settings_.keepAliveUntilTimeMs);
            if (eventStatus_ > 0)
            {
                peer_ = event_.peer;
                switch (event_.type)
                {
                    case ENET_EVENT_TYPE_CONNECT:
                        enet_address_get_host_ip(&event_.peer->address, buffer, 128);
                        if (settings_.logTransitMsgs) printf("SNode[0]:Node connected[%s]:srcId:[%u]\n", buffer, event_.data);
                        peerMap_[event_.data] = event_.peer;
                        break;

                    case ENET_EVENT_TYPE_RECEIVE:
                        enet_address_get_host_ip(&event_.peer->address, buffer, 128);
                        memcpy(&m, event_.packet->data, sizeof(Message_t));

                        if (settings_.logTransitMsgs) printf("SNode[0]:[%u -> %u]:seq[%u]:mid[%d]", m.srcId, m.dstId, m.seq, m.mid);

                        if (0==m.dstId)
                        {
                            handleServerMsg_(&m);
                        }
                        else
                        {
                            if (peerMap_.count(m.dstId) > 0)
                            {
                                if (enet_peer_send(peerMap_[m.dstId], event_.channelID, event_.packet) < 0)
                                {
                                    if (settings_.logTransitMsgs) printf(":Failed\n");
                                    enet_packet_destroy(event_.packet);
                                }
                                else
                                {
                                    if (settings_.logTransitMsgs) printf("\n");	
                                }
                            }
                            else
                            {
                                if (settings_.logTransitMsgs) printf(":NoPeer\n");	
                                enet_packet_destroy(event_.packet);
                            }
                        }
                        break;

                    case ENET_EVENT_TYPE_DISCONNECT:
                        enet_address_get_host_ip(&event_.peer->address, buffer, 128);
                        if (settings_.logTransitMsgs) printf("SNode[0]:Node disconnected[%s]:srcId:[%u]\n", buffer, event_.data);
                        event_.peer->data = NULL;
                        peerMap_[event_.data] = NULL;
                        break;

                    case ENET_EVENT_TYPE_NONE:
                        if (settings_.logTransitMsgs) printf("SNode[0]:ENET_EVENT_TYPE_NONE\n");
                }
            }
        }
        else
        {
            eventStatus_ = enet_host_service(host_, &event_, settings_.keepAliveUntilTimeMs);
            if (eventStatus_ > 0)
            {
                switch (event_.type)
                {
                    case ENET_EVENT_TYPE_CONNECT:
                        enet_address_get_host_ip(&event_.peer->address, buffer, 128);
                        if (settings_.logTransitMsgs) printf("CNode[%u]:Node connected[%s]:srcId:[%u]\n", settings_.nodeId, buffer, event_.data);
                        break;

                    case ENET_EVENT_TYPE_RECEIVE:
                        enet_address_get_host_ip(&event_.peer->address, buffer, 128);
                        memcpy(&m, event_.packet->data, sizeof(Message_t));

                        if (settings_.logTransitMsgs) printf("CNode[%u]:[%u -> %u]:seq[%u]:mid[%d]\n", m.dstId, m.srcId, settings_.nodeId, m.seq, m.mid);
                        if (0==m.dstId)
                        {
                        }
                        else if (m.dstId == settings_.nodeId)
                        {
                            if (m.isRspMsg)
                            {
                                rspMsg_.push_back(m);
                                if (rspMsg_.size() > settings_.maxRspMsgSize)
                                {
                                    rspMsg_.erase(rspMsg_.begin());
                                }
                            }
                            else
                            {
                                newMsg_.push_back(m);
                                if (newMsg_.size() > settings_.maxNewMsgSize)
                                {
                                    newMsg_.erase(newMsg_.begin());
                                }
                            }
                        }
                        enet_packet_destroy(event_.packet);
                        break;

                    case ENET_EVENT_TYPE_DISCONNECT:
                        enet_address_get_host_ip(&event_.peer->address, buffer, 128);
                        if (settings_.logTransitMsgs) printf("CNode[%u]:Node disconnected[%s]:srcId:[%u]\n", settings_.nodeId, buffer, event_.data);
                        event_.peer->data = NULL;
                        peerMap_[event_.data] = NULL;
                        break;

                    case ENET_EVENT_TYPE_NONE:
                        if (settings_.logTransitMsgs) printf("SNode[0]:ENET_EVENT_TYPE_NONE\n");
                }
            }
        }
    }
    return Status_t::PL_OK;
}

PObject::Status_t NetworkNode::send(Message_t* msg, bool updateSeq)
{
    if (strlen(msg->payload) > 0)
    {
        if (updateSeq) msg->seq = msgSeq_++;
        ENetPacket *packet = enet_packet_create(msg, sizeof(Message_t), ENET_PACKET_FLAG_RELIABLE);
        if (enet_peer_send(peer_, channel_, packet) < 0)
        {
            if (settings_.logTransitMsgs) printf("CNode[%u]:[%u -> %u]:seq[%u]:mid[%d]\n", settings_.nodeId, msg->srcId, msg->dstId, msg->seq, msg->mid);
            enet_packet_destroy(packet);
            return Status_t::PL_NOT_OK;
        }
    }
    else
    {
        return Status_t::PL_NOT_OK;
    }
    return Status_t::PL_OK;
}

PObject::Status_t NetworkNode::send(int nodeId, int mid, char* payload, int length)
{
    Message_t m = {0};
    m.dstId = nodeId;
    m.srcId = settings_.nodeId;
    m.mid = mid;
    m.rspNeeded = false;
    m.isRspMsg = false;
    snprintf(m.payload, (length > NETWORK_NODE_MSG_PAYLOAD_SIZE) ? NETWORK_NODE_MSG_PAYLOAD_SIZE : length, "%s", payload);
    send(&m);
    return Status_t::PL_OK;
}

PObject::Status_t NetworkNode::send(int nodeId, int mid, const char* payload)
{
    Message_t m = {0};
    m.dstId = nodeId;
    m.srcId = settings_.nodeId;
    m.mid = mid;
    m.rspNeeded = false;
    m.isRspMsg = false;
    snprintf(m.payload, NETWORK_NODE_MSG_PAYLOAD_SIZE, "%s", payload);
    return send(&m);
}

PObject::Status_t NetworkNode::sendQuery(int nodeId, int mid, char* payload, int length, Message_t* rspMsg, uint64_t timeoutMs)
{
    if (rspMsg == NULL) return Status_t::PL_NOT_OK;
    Message_t m = {0};
    m.dstId = nodeId;
    m.srcId = settings_.nodeId;
    m.mid = mid;
    m.rspNeeded = true;
    m.isRspMsg = false;
    snprintf(m.payload, (length > NETWORK_NODE_MSG_PAYLOAD_SIZE) ? NETWORK_NODE_MSG_PAYLOAD_SIZE : length, "%s", payload);
    if (Status_t::PL_NOT_OK == send(&m)) return Status_t::PL_NOT_OK;
    rspMsg->seq = m.seq;
    return recvQuery(rspMsg, timeoutMs);
}

PObject::Status_t NetworkNode::sendQuery(int nodeId, int mid, const char* payload, Message_t* rspMsg, uint64_t timeoutMs)
{
    if (rspMsg == NULL) return Status_t::PL_NOT_OK;
    Message_t m = {0};
    m.dstId = nodeId;
    m.srcId = settings_.nodeId;
    m.mid = mid;
    m.rspNeeded = true;
    m.isRspMsg = false;
    snprintf(m.payload, NETWORK_NODE_MSG_PAYLOAD_SIZE, "%s", payload);
    if (Status_t::PL_NOT_OK == send(&m)) return Status_t::PL_NOT_OK;
    rspMsg->seq = m.seq;
    return recvQuery(rspMsg, timeoutMs);
}

PObject::Status_t NetworkNode::respQuery(int nodeId, int mid, char* payload, int length, uint8_t seq)
{
    Message_t m = {0};
    m.dstId = nodeId;
    m.srcId = settings_.nodeId;
    m.mid = mid;
    m.rspNeeded = false;
    m.isRspMsg = true;
    m.seq = seq;
    snprintf(m.payload, (length > NETWORK_NODE_MSG_PAYLOAD_SIZE) ? NETWORK_NODE_MSG_PAYLOAD_SIZE : length, "%s", payload);
    return send(&m, false);
}

PObject::Status_t NetworkNode::respQuery(int nodeId, int mid, const char* payload, uint8_t seq)
{
    Message_t m = {0};
    m.dstId = nodeId;
    m.srcId = settings_.nodeId;
    m.mid = mid;
    m.rspNeeded = false;
    m.isRspMsg = true;
    m.seq = seq;
    snprintf(m.payload, NETWORK_NODE_MSG_PAYLOAD_SIZE, "%s", payload);
    return send(&m, false);
}

PObject::Status_t NetworkNode::recv(Message_t* msg)
{
    if (newMsg_.size() == 0) return Status_t::PL_NOT_OK;
    auto m = newMsg_.front();
    memcpy(msg, &m, sizeof(Message_t));
    newMsg_.erase(newMsg_.begin());
    return Status_t::PL_OK;
}

PObject::Status_t NetworkNode::recvQuery(Message_t* msg, uint64_t timeoutMs)
{
    if (msg == NULL) return Status_t::PL_NOT_OK;

    uint64_t t0 = PObject::getTickMs() + (float)timeoutMs/getTickPeriodMs();
    while (PObject::getTickMs() < t0)
    {
        for (int i = 0; i < (int)rspMsg_.size(); ++i)
        {
            if (rspMsg_[i].seq == msg->seq)
            {
                memcpy(msg, &rspMsg_[i], sizeof(Message_t));
                return Status_t::PL_OK;
            }
        }
    }
    if (settings_.logTransitMsgs) printf("recvQuery:timeout\n");
    return Status_t::PL_NOT_OK;
}

PObject::Status_t NetworkNode::shutdown(void)
{
    if (shutdownCalled_) return Status_t::PL_OK;
    shutdownCalled_ = true;
    enet_host_destroy(host_);
    atexit(enet_deinitialize);
    return Status_t::PL_OK;
}

void NetworkNode::handleServerMsg_(Message_t* m)
{
    if (m->mid == GET_NODES && m->rspNeeded)
    {
        if (settings_.logTransitMsgs) printf(":%s\n", MessageIdStr[DEBUG]);
        int idx = 0;
        char payload[1024] = {0};
        payload[idx++] = peerMap_.size();

        for (const auto& pair : peerMap_)
        {
            payload[idx++] = pair.first;
        }
        respQuery(m->srcId, m->mid, payload, peerMap_.size() + 1, m->seq);
    }
    else
    {
        enet_host_broadcast(host_, event_.channelID, event_.packet);
        enet_packet_destroy(event_.packet);
        if (settings_.logTransitMsgs) printf(":Bcast\n");
    }
}

AppSettings::AppSettings(const char* name) : UtilsObject(name)
{
}

PObject::Status_t AppSettings::initialize(void)
{
    std::ifstream fileStream(settings_.file);
    char json[1024] = {0};
    if (fileStream.is_open())
    {
        std::string nodesStr((std::istreambuf_iterator<char>(fileStream)),
                              std::istreambuf_iterator<char>());
        fileStream.close();
        sprintf(json, "%s", nodesStr.c_str());
    }
    else
    {
        std::cerr << "Error: Unable to open file."<< settings_.file << std::endl;
        return Status_t::PL_NOT_OK;
    }

    data_.doc.Parse(json);
    return Status_t::PL_OK;
}

PObject::Status_t AppSettings::run(void)
{
    return Status_t::PL_OK;
}

PObject::Status_t AppSettings::shutdown(void)
{
    return Status_t::PL_OK;
}