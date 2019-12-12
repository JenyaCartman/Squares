#pragma once

#include <enet/enet.h>
#include <string>

class ENetClient
{
public:
    ENetClient();
    ~ENetClient();

    bool Connect();
    void Send(void* message);
    void Disconnect();

    bool CheckMessages();

    enet_uint32 GetConnectID() const;
    std::string GetLastMessage() const;

private:
    ENetHost* m_client;
    ENetPeer* m_peer;

    std::string m_message;

};

