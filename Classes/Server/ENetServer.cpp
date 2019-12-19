#include "ENetServer.h"
#include <iostream>


ENetServer::ENetServer() : clientsCounter(0), m_currentTick(0), m_tickRate(3)
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


void ENetServer::Send(void* message, ENetPeer* peer)
{
    ENetPacket *packet = enet_packet_create(message, strlen(static_cast<char*>(message)) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}


void ENetServer::Listen()
{
    ENetEvent event;
    OutputMemoryStream responseStream;
    while (enet_host_service(m_server, &event, 5) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            //OutputDebugStringA("A new client connected with ID " +  event.peer->connectID);
                        event.peer->data = reinterpret_cast<void*>(++clientsCounter);
                        m_worldState.push_back(PlayerInfo(clientsCounter));

            break;

        case ENET_EVENT_TYPE_RECEIVE:
        {
            void* temporaryBuffer = std::malloc(event.packet->dataLength);
            memcpy(temporaryBuffer, event.packet->data, event.packet->dataLength);
            InputMemoryStream stream(static_cast<enet_uint8*>(temporaryBuffer), event.packet->dataLength);

            ReadMessage(event.packet->data, event.packet->dataLength, reinterpret_cast<size_t>(event.peer->data));
            // Send(responseStream.GetBufferPtr(),event.peer);
            enet_packet_destroy(event.packet);
            UpdateWorldState();
        }
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            OutputDebugStringA(event.peer->connectID + " disconnected.\n");

            auto player = std::find_if(m_worldState.begin(), m_worldState.end(),
                [&](PlayerInfo player) { return event.peer->data == reinterpret_cast<void*>(player.ID); });
            if (player != m_worldState.end())
            {
                m_worldState.erase(player);
            }

            /* Reset the peer's client information. */
            event.peer->data = NULL;
        }
    }

    m_currentTick++;
    if (m_currentTick == m_tickRate)
    {
        SendUpdates();
        m_currentTick = 0;
    }
}


void ENetServer::SendUpdates()
{
    OutputMemoryStream stream;
    SerializeWorldState(stream);
    ENetPacket * packet = enet_packet_create(stream.GetBufferPtr(), stream.GetLength() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(m_server, 0, packet);
}


 void ENetServer::ProcessMessage(const ClientRequest& request, OutputMemoryStream& stream)
{
     auto player = std::find_if(m_worldState.begin(), m_worldState.end(),
         [&](PlayerInfo player) { return request.ID == player.ID; });

    stream.Write(request.index);
    stream.Write(player->ID);
    stream.Write(player->square);
}


void ENetServer::SerializeWorldState(OutputMemoryStream& stream) const
{        
    stream.Write(m_worldState.size());

    for (const auto& player : m_worldState)
    {
        stream.Write(player.ID);
        stream.Write(player.square);
    }
}


void ENetServer::ReadMessage(enet_uint8* message, size_t length, size_t id)
{
    void* temporaryBuffer = std::malloc(length);
    memcpy(temporaryBuffer, message, length);
    InputMemoryStream stream(static_cast<enet_uint8*>(temporaryBuffer), length);

    size_t size;
    stream.Read(size);

    for (size_t i = 0; i < size; ++i)
    {
        ClientRequest request;
        request.ID = id;
        stream.Read(request.time);
        stream.Read(request.direction);

        m_requests.push_back(request);
    }
}


void ENetServer::UpdateWorldState()
{
    for (const auto& request : m_requests)
    {
        auto player = std::find_if(m_worldState.begin(), m_worldState.end(),
            [&](PlayerInfo player) { return request.ID == player.ID; });

        if (player != m_worldState.end())
        {
            player->square += request.direction * params::speed * request.time;
        }
    }

    m_requests.clear();
}


const std::vector<PlayerInfo>& ENetServer::GetWorldState() const
{
    return m_worldState;
}