#pragma once

#include "networking_shared.h"

class ClientGC;
class GCMessageRead;
class GCMessageWrite;

struct AuthTicket
{
    uint64_t steamId{};       // gameserver we ended up talking to (set on validate)
    uint64_t serverSteamId{}; // gameserver we requested the ticket for (set on create), 0 if unknown
    std::vector<uint8_t> buffer;
};

class NetworkingClient
{
public:
    NetworkingClient(ISteamNetworkingMessages *networkingMessages);

    void Update(ClientGC *gc);

    void SendMessage(const void *data, uint32_t size);

    // for gameserver validation. serverSteamId is the SteamID we requested the
    // ticket for, if known (0 if unknown). it lets the validator accept the
    // server-side handshake when Steam mutates the ticket bytes in transit
    // (eg. server using csgo_gc auth bypass — see issues #45/#67).
    void SetAuthTicket(uint32_t handle, const void *data, uint32_t size, uint64_t serverSteamId = 0);
    void ClearAuthTicket(uint32_t handle);

private:
    // return false if it wasn't handled, in which case we pass it to m_clientGC
    bool HandleMessage(ClientGC *gc, uint64_t steamId, GCMessageRead &message);

    ISteamNetworkingMessages *const m_networkingMessages;
    uint64_t m_serverSteamId{};

    std::unordered_map<uint32_t, AuthTicket> m_tickets;

    STEAM_CALLBACK(NetworkingClient,
        OnSessionRequest,
        SteamNetworkingMessagesSessionRequest_t,
        m_sessionRequest);

    STEAM_CALLBACK(NetworkingClient,
        OnSessionFailed,
        SteamNetworkingMessagesSessionFailed_t,
        m_sessionFailed);
};
