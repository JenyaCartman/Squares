#pragma once

#include <vector>

#include "PlayerInfo.h"
#include "MessageInfo.h"
#include "InputMemoryStream.h"
#include "OutputMemoryStream.h"

class ENetClient
{
public:
    ENetClient();
    ~ENetClient();

    bool Connect();
    void Send(void* message);
    void Disconnect();

    bool ReceiveMessage();

    size_t GetID() const;
    const std::vector<PlayerInfo>& GetWorldState() const;
    size_t GetRequestsSize() const;

    void AddRequest(ClientRequest request);
    void ProcessRequests();
    void SerializeRequests(OutputMemoryStream& stream);

private:
    void ReadWorldState(InputMemoryStream& stream);

private:
    ENetHost* m_client;
    ENetPeer* m_peer;

    std::vector<PlayerInfo> m_worldState;
    std::vector<ClientRequest> m_requests;
};

