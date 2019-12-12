#include "ENetServer.h"
#include "cocos2d.h"
#include <iostream>


ENetServer::ENetServer()
{
    if (enet_initialize() != 0)
    {
        OutputDebugStringA("\nAn error occurred while initializing ENet.\n");
    }

    atexit(enet_deinitialize);

    m_address.host = ENET_HOST_ANY;
    m_address.port = 5000;

    m_server = enet_host_create(&m_address, 32, 2, 0, 0);

    if (m_server == nullptr)
    {
        OutputDebugStringA("\nAn error occurred while trying to create an ENet server host.\n");
    }
}


ENetServer::~ENetServer()
{
    enet_host_destroy(m_server);
}


void ENetServer::Listen()
{
    ENetEvent event;
    while (enet_host_service(m_server, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
           // OutputDebugStringA("A new client connected with ID " +  event.peer->connectID);
            m_worldState.push_back(PlayerInfo(event.peer->connectID));
            SendUpdates();
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            //std::cout << "A packet containing " << event.packet->dataLength%s was received from %s on channel %u.\n",
            //    
            //    event.packet->data,
            //    event.peer->data,
            //    event.channelID);

            /* Clean up the packet now that we're done using it. */

            if (strcmp((char*)event.packet->data, "left") == 0)
                ProcessMessage(EMessageType::Left, event.peer->connectID);
            else if (strcmp((char*)event.packet->data, "right") == 0)
                ProcessMessage(EMessageType::Right, event.peer->connectID);
            else if (strcmp((char*)event.packet->data, "up") == 0)
                ProcessMessage(EMessageType::Up, event.peer->connectID);
            else if (strcmp((char*)event.packet->data, "down") == 0)
                ProcessMessage(EMessageType::Down, event.peer->connectID);

            enet_packet_destroy(event.packet);

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", event.peer->data);

            /* Reset the peer's client information. */
            event.peer->data = NULL;
        }
    }
}


void ENetServer::SendUpdates()
{
    std::string message;

    for (const auto& client : m_worldState)
    {
        message.append(client.CreateMessage());
        message.append(".");
    }

    ENetPacket * packet = enet_packet_create(message.c_str(), message.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(m_server, 0, packet);
}


void ENetServer::ProcessMessage(EMessageType type, enet_uint32 id)
{
    auto& player = FindClient(id);

    switch (type)
    {
    case EMessageType::Left:
        player.points[0].x -= 5;
        player.points[1].x -= 5;
        player.points[2].x -= 5;
        player.points[3].x -= 5;
        break;
    case EMessageType::Right:
        player.points[0].x += 5;
        player.points[1].x += 5;
        player.points[2].x += 5;
        player.points[3].x += 5;
        break;
    case EMessageType::Up:
        player.points[0].y += 5;
        player.points[1].y += 5;
        player.points[2].y += 5;
        player.points[3].y += 5;
        break;
    case EMessageType::Down:
        player.points[0].y -= 5;
        player.points[1].y -= 5;
        player.points[2].y -= 5;
        player.points[3].y -= 5;
        break;
    }

    SendUpdates();
}


PlayerInfo& ENetServer::FindClient(enet_uint32 id)
{
    for (auto& client : m_worldState)
    {
        if (client.connectID == id)
        {
            return client;
        }
    }
}


const std::vector<PlayerInfo>& ENetServer::GetWorldState() const
{
    return m_worldState;
}