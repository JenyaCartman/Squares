#pragma once
#include <enet/enet.h>
#include <vector>

#include "PlayerInfo.h"

enum class EMessageType
{
    Connect = 0,
    Left,
    Right,
    Up,
    Down
};

class ENetServer
{
public:
    ENetServer();
    ~ENetServer();

    void Listen();
    const std::vector<PlayerInfo>& GetWorldState() const;

private:
    void SendUpdates();
    void ProcessMessage(EMessageType type, enet_uint32 id);

    PlayerInfo& FindClient(enet_uint32 id);

private:
    ENetAddress m_address;
    ENetHost* m_server;

    std::vector<PlayerInfo> m_worldState;
};