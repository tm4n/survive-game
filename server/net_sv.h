#ifndef __NET_SV_H__
#define __NET_SV_H__

#include "net.h"
#include "actor.h"

// data structure stored with each ENetPeer on server:

struct s_peer_data
{
    uint clstate;
    uint player_actor_id;
    char *player_name;
};

///////////////////////////////////////////////
// special send functions for server

int net_send_version(int version, ENetPeer *receiver);
int net_send_sync_server(const char *mapfile, ENetPeer *receiver);

int net_broadcast_chat(const char* msg, uint len);

int net_send_sync_player(uint actor_id, const char *name, ENetPeer *receiver);
int net_broadcast_sync_player(uint actor_id, const char *name);

int net_send_sync_box(uint actor_id, vec *pos, uint health, ENetPeer *receiver);
int net_broadcast_sync_box(uint actor_id, char box_type, vec *pos, uint health);

int net_send_sync_finish(ENetPeer *receiver);

int net_broadcast_remove_actor(uint actor_id);

int net_send_update_state(uint actor_id, ushort state, ENetPeer *receiver);
int net_send_update_target(uint actor_id, uint target, ENetPeer *receiver);
int net_send_update_pos(uint actor_id, vec *pos, ENetPeer *receiver);
int net_send_update_health(uint actor_id, float health, ENetPeer *receiver);

#endif // __NET_SV_H__