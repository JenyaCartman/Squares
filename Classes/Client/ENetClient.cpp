#include "Client/ENetClient.h"

ENetClient::ENetClient()
{
    if (enet_initialize() != 0)
    {
        OutputDebugStringA("\nAn error occurred while initializing ENet.\n");
    }

    atexit(enet_deinitialize);

    m_client = enet_host_create(NULL, 1, 2, 0, 0);

    if (m_client == nullptr)
    {
        OutputDebugStringA("An error occurred while trying to create an ENet client host.\n");
    }
}


ENetClient::~ENetClient()
{
    Disconnect();
    enet_host_destroy(m_client);
}


bool ENetClient::Connect()
{
    ENetAddress address;
    ENetEvent event;

    enet_address_set_host(&address, "localhost");
    address.port = 5000;

    m_peer = enet_host_connect(m_client, &address, 1, 0);

    if (m_peer == nullptr)
    {
        OutputDebugStringA("No available peers for initializing an ENet connection.\n");
        return false;
    }

    if (enet_host_service(m_client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        OutputDebugStringA("Connection to localhost:5000 succeeded.\n");
    }
    else
    {
        enet_peer_reset(m_peer);
        OutputDebugStringA("Connection to localhost:5000 failed.\n");
        return false;
    }

    return true;
}


void ENetClient::Send(void* message)
{
    ENetPacket *packet = enet_packet_create(message, strlen(static_cast<char*>(message)) + 1, ENET_PACKET_FLAG_RELIABLE); 
    enet_peer_send(m_peer, 0, packet);
}


void ENetClient::Disconnect()
{
    ENetEvent event;

    enet_peer_disconnect(m_peer, 0);

    while (enet_host_service(m_client, &event, 5000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            OutputDebugStringA("Disconnection succeeded.\n");
            return;
        }
    }

    enet_peer_reset(m_peer);
}


bool ENetClient::ReceiveMessage()
{
    ENetEvent event;
    while (enet_host_service(m_client, &event, 5) > 0)
    {
        if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            void* temporaryBuffer = std::malloc(event.packet->dataLength);
            memcpy(temporaryBuffer, event.packet->data, event.packet->dataLength);
            InputMemoryStream stream(static_cast<enet_uint8*>(temporaryBuffer), event.packet->dataLength);
            ReadWorldState(stream);

            enet_packet_destroy(event.packet);
            return true;
        }
    }

    return false;
}


void ENetClient::ReadWorldState(InputMemoryStream& stream)
{
    size_t size;
    stream.Read(size);
    m_worldState.clear();

    for (size_t i = 0; i < size; ++i)
    {
        PlayerInfo tmp(0);
        stream.Read(tmp.ID);
        stream.Read(tmp.square);
        m_worldState.push_back(tmp);
    }
}


void ENetClient::ProcessRequests()
{
    if (!m_requests.empty())
    {
        OutputMemoryStream stream;
        SerializeRequests(stream);

        void* message = stream.GetBufferPtr();
        ENetPacket *packet = enet_packet_create(message, stream.GetLength() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(m_peer, 0, packet);

        m_requests.clear();
    }
}


void ENetClient::SerializeRequests(OutputMemoryStream& stream)
{
    size_t size = m_requests.size();
    stream.Write(size);

    for (size_t i = 0; i < size; ++i)
    {
        stream.Write(m_requests[i].time);
        stream.Write(m_requests[i].direction);
    }
}


void ENetClient::AddRequest(ClientRequest request)
{
    m_requests.push_back(request);
}


size_t ENetClient::GetID() const
{
    return reinterpret_cast<size_t>(m_peer->data);
}


const std::vector<PlayerInfo>& ENetClient::GetWorldState() const
{
    return m_worldState;
}


size_t ENetClient::GetRequestsSize() const
{
    return m_requests.size();
}