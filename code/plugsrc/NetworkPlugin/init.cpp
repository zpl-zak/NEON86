// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <unordered_map>

#include <NeonEngine.h>
#include <lua_macros.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ENET_IMPLEMENTATION
#include "enet.h"

#include <lua/lua.hpp>

ENetHost *server = NULL;
ENetHost *client = NULL;
ENetPeer *client_peer = NULL;


static INT ne_server_start(lua_State* L) {
    if (server) {
        OutputDebugStringA("[server] Server is already running...\n");
        lua_pushnumber(L, -1);
        return 1;
    }

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = 27020; /* Bind the server to port . */

    /* create a server */
    server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == NULL) {
        OutputDebugStringA("[server] An error occurred while trying to create an ENet server host.\n");
        lua_pushnumber(L, -1);
        return 1;
    }

    OutputDebugStringA("[server] Started an ENet server...\n");
    lua_pushnumber(L, 1);
    return 1;
}

static INT ne_server_stop(lua_State* L) {
    if (!server) {
        lua_pushnumber(L, -1);
        return 1;
    }

    enet_host_destroy(server);
    server = NULL;

    lua_pushnumber(L, 1);
    return 1;
}

static INT ne_connect(lua_State* L) {
    if (client || client_peer) {
        OutputDebugStringA("[client] You are already connected, disconnect first\n");
        lua_pushnumber(L, -1);
        return 1;
    }

    const char* hoststr = luaL_checkstring(L, 1);

    ENetAddress address = {0}; address.port = 27020;
    enet_address_set_host(&address, hoststr);

    client = enet_host_create(NULL, 1, 2, 0, 0);
    client_peer = enet_host_connect(client, &address, 2, 0);

    if (client_peer == NULL) {
        OutputDebugStringA("[client] Cannot connect\n");
        lua_pushnumber(L, -1);
        return 1;
    }

    lua_pushnumber(L, 1);
    return 1;
}

static INT ne_disconnect(lua_State* L) {
    if (!client || !client_peer) {
        lua_pushnumber(L, -1);
        return 1;
    }

    enet_peer_disconnect_now(client_peer, 0);
    enet_host_destroy(client);

    client_peer = NULL;
    client = NULL;

    lua_pushnumber(L, 1);
    return 1;
}

typedef struct {
    float x, y, z, r;
} ne_data;


std::unordered_map<uint64_t, ne_data> ne_server_data;

void ne_server_update() {
    ENetEvent event = {0};
    while (enet_host_service(server, &event, 2) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                OutputDebugStringA("[server] A new user connected.\n");
                uint16_t entity_id = event.peer->incomingPeerID;

                /* allocate and store entity data in the data part of peer */
                ne_data _ent = {0};
                ne_server_data[entity_id] = _ent;
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                OutputDebugStringA("[server]  A user disconnected.\n");
                uint16_t entity_id = event.peer->incomingPeerID;

                ne_server_data.erase(entity_id);
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                /* handle a newly received event */
                uint16_t entity_id = event.peer->incomingPeerID;
                char *buffer = (char *)event.packet->data;
                int offset = 0;

                float x = *(float*)(buffer + offset); offset += sizeof(float);
                float y = *(float*)(buffer + offset); offset += sizeof(float);
                float z = *(float*)(buffer + offset); offset += sizeof(float);
                float r = *(float*)(buffer + offset); offset += sizeof(float);

                ne_server_data[entity_id].x = x;
                ne_server_data[entity_id].y = y;
                ne_server_data[entity_id].z = z;
                ne_server_data[entity_id].r = r;

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }

    // static float last_send = 0;
    // float diff_send = GetTime() - last_send;
    // last_send = GetTime();

    // /* every 030 ms */
    // if (diff_send < 0.030) { return; }

    /* iterate peers and send them updates */
    ENetPeer *currentPeer;
    for (currentPeer = server->peers; currentPeer < &server->peers[server->peerCount]; ++currentPeer) {
        if (currentPeer->state != ENET_PEER_STATE_CONNECTED) {
            continue;
        }

        int offset = sizeof(uint32_t);
        int count = 0;
        char buffer[4096] = {0};

        for (auto it = ne_server_data.begin(); it != ne_server_data.end(); ++it) {
            if (currentPeer->incomingPeerID == it->first) continue; /* skip sending to local player */
            *(uint16_t*)(buffer + offset) = it->first; offset += sizeof(uint16_t);
            *(float*)(buffer + offset) = it->second.x; offset += sizeof(float);
            *(float*)(buffer + offset) = it->second.y; offset += sizeof(float);
            *(float*)(buffer + offset) = it->second.z; offset += sizeof(float);
            *(float*)(buffer + offset) = it->second.r; offset += sizeof(float);
            count++;
        }

        *(uint32_t*)(buffer) = count;

        if (offset > 0) {
            /* create packet with actual length, and send it */
            ENetPacket *packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(currentPeer, 0, packet);
        }
    }
}

void ne_client_update(lua_State* L) {
    ENetEvent event = {0};

    ENetPeer *peer = client_peer;
    ENetHost *host = client;

    while (enet_host_service(host, &event, 2) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                OutputDebugStringA("[client] We connected to the server.\n");
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                OutputDebugStringA("[client] We disconnected from server.\n");
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                /* handle a newly received event */
                int offset = 0;
                char *buffer = (char *)event.packet->data;
                int count = *(uint32_t*)(buffer); offset += sizeof(uint32_t);

                for (int i = 0; i < count; ++i) {
                    uint16_t entity_id = *(uint16_t*)(buffer + offset); offset += sizeof(uint16_t);
                    float x = *(float*)(buffer + offset); offset += sizeof(float);
                    float y = *(float*)(buffer + offset); offset += sizeof(float);
                    float z = *(float*)(buffer + offset); offset += sizeof(float);
                    float r = *(float*)(buffer + offset); offset += sizeof(float);

                    // OutputDebugStringA("update: %ld: [%f %f %f] %f\n", entity_id, x, y, z, r);

                    lua_getglobal(L, "_net_tankupdate");

                    if (!lua_isfunction(L, -1))
                        continue;

                    lua_pushnumber(L, entity_id);
                    lua_pushnumber(L, x);
                    lua_pushnumber(L, y);
                    lua_pushnumber(L, z);
                    lua_pushnumber(L, r);

                    lua_pcall(L, 5, 0, 0);
                }

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }
}

static INT ne_update(lua_State* L) {
    if (server) ne_server_update();
    if (client) ne_client_update(L);
    lua_pushnumber(L, 1);
    return 1;
}

static INT ne_send(lua_State* L) {
    if (!client || !client_peer) {
        lua_pushnumber(L, -1);
        return 1;
    }

    /* send our data to the server */
    char buffer[256] = {0};
    size_t offset = 0;

    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float z = luaL_checknumber(L, 3);
    float r = luaL_checknumber(L, 4);

    /* serialize peer's the world view to a buffer */
    *(float*)(buffer + offset) = x; offset += sizeof(float);
    *(float*)(buffer + offset) = y; offset += sizeof(float);
    *(float*)(buffer + offset) = z; offset += sizeof(float);
    *(float*)(buffer + offset) = r; offset += sizeof(float);

    /* create packet with actual length, and send it */
    ENetPacket *packet = enet_packet_create(buffer, offset, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(client_peer, 0, packet);

    lua_pushnumber(L, 1);
    return 1;
}

static const luaL_Reg networkplugin[] = {
    {"server_start", ne_server_start},
    {"server_stop", ne_server_stop},
    {"connect", ne_connect},
    {"disconnect", ne_disconnect},
    {"update", ne_update},
    {"send", ne_send},
    ENDF
};

extern "C" INT PLUGIN_API luaopen_networkplugin(lua_State* L) {
    enet_initialize();
    luaL_newlib(L, networkplugin);
    return 1;
}
