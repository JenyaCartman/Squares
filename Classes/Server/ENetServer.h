#pragma once
#include <vector>
#include <ctime>

#include "PlayerInfo.h"
#include "MessageInfo.h"
#include "OutputMemoryStream.h"
#include "InputMemoryStream.h"
#include "Common/SharedConfig.h"

class ENetServer
{
public:
    ENetServer();
    ~ENetServer();

    void Listen();
    const std::vector<PlayerInfo>& GetWorldState() const;

private:
    void Send(void* message, ENetPeer* peer);

    void SendUpdates();
    void SerializeWorldState(OutputMemoryStream& stream) const;

    void ReadMessage(enet_uint8* message, size_t length, size_t id);
    void ProcessMessage(const ClientRequest& request, OutputMemoryStream& stream);

    void UpdateWorldState();

private:
    ENetAddress m_address;
    ENetHost* m_server;

    int m_currentTick;
    int m_tickRate;

    size_t clientsCounter;
    Snapshot m_worldState;
    std::vector<ClientRequest> m_requests;
};