#include "net_sv.h"
#include "vec.h"

#include "string.h" // for memcpy, strcpy



/////////////////////////////////////////////////////////////////
// specialized send functions
// server

int net_send_version(int version, ENetPeer *receiver)
{
    s_net_version s;

    s.version = version;

    return net_send_event(NET_VERSION, (const char *)&s, sizeof(s_net_version), receiver);
}


int net_send_sync_server(const char *mapfile, ENetPeer *receiver)
{
    s_net_sync_server s;

    strncpy(s.mapfile, mapfile, 32);
    s.mapfile[31] = '\0';

    return net_send_event(NET_SYNC_SERVER, (const char *)&s, sizeof(s_net_sync_server), receiver);
}

int net_broadcast_chat(const char* msg, uint len)
{
	return net_broadcast_event(NET_CHAT, msg, len, gEhost);
}


int net_send_sync_player(uint actor_id, const char *name, ENetPeer *receiver)
{
    s_net_sync_player s;

	// ..

    printf("net_sync_player mit actor_id=%u, name=%s \n", actor_id, name);

    return net_send_event(NET_SYNC_PLAYER, (const char *)&s, sizeof(s_net_sync_player), receiver);

}

int net_broadcast_sync_player(uint actor_id, const char *name, ENetHost *host)
{
    // TODO: broadcast only to synchronized players?
    s_net_sync_player s;

    /// ..

    printf("net_sync_player_broadcast mit actor_id=%u, name=%s \n", actor_id,  name);

    return net_broadcast_event(NET_SYNC_PLAYER, (const char *)&s, sizeof(s_net_sync_player), host);
}

int net_send_sync_box(uint actor_id, vec *pos, uint health, ENetPeer *receiver)
{
    s_net_sync_box s;

    s.actor_id = actor_id;
    s.pos.set(pos);
    s.health = health;

    printf("net_sync_box mit actor_id=%un", actor_id);

    return net_send_event(NET_SYNC_BOX, (const char *)&s, sizeof(s_net_sync_box), receiver);

}

int net_broadcast_sync_box(uint actor_id, char box_type, vec *pos, uint health)
{
    // TODO: broadcast only to synchronized players?
    s_net_sync_box s;

    s.actor_id = actor_id;
    s.box_type = box_type;
    s.pos.set(pos);
    s.health = health;

    printf("net_sync_box broadcast mit actor_id=%u\n", actor_id);

    return net_broadcast_event(NET_SYNC_BOX, (const char *)&s, sizeof(s_net_sync_box), gEhost);

}

int net_send_sync_finish(ENetPeer *receiver)
{
	return net_send_event(NET_SYNC_FINISH, NULL, 0, receiver);
}

int net_broadcast_remove_actor(uint actor_id)
{
    // TODO: broadcast only to synchronized players?
    s_net_remove_actor s;

	// no input yet

    printf("net_sync_remove_broadcast mit actor_id=%u\n", actor_id);

    return net_broadcast_event(NET_REMOVE_ACTOR, (const char *)&s, sizeof(s_net_remove_actor), gEhost);
}

int net_send_sync_finish(uint own_actor_id, ENetPeer *receiver)
{

    printf("net_sync_vfinish\n");

    return net_send_event(NET_SYNC_FINISH, NULL, NULL, receiver);
}



// update values
int net_send_update_state(uint actor_id, ushort state, ENetPeer *receiver)
{
    s_net_update_state s;

    s.actor_id = actor_id;
    s.state = state;

    return net_send_event(NET_UPDATE_STATE, (const char*) &s, sizeof(s_net_update_state), receiver);
}

int net_send_update_target(uint actor_id, uint target, ENetPeer *receiver)
{
    s_net_update_target s;

    s.actor_id = actor_id;
    s.target = target;

    return net_send_event(NET_UPDATE_TARGET, (const char*) &s, sizeof(s_net_update_target), receiver);
}

int net_send_update_pos(uint actor_id, vec *pos, ENetPeer *receiver)
{
    s_net_update_pos s;

    s.actor_id = actor_id;
    s.x = pos->x;
    s.y = pos->y;

    return net_send_event(NET_UPDATE_POS, (const char*) &s, sizeof(s_net_update_pos), receiver);
}

int net_send_update_health(uint actor_id, float health, ENetPeer *receiver)
{
    s_net_update_health s;

    s.actor_id = actor_id;
    s.health = health;

    return net_send_event(NET_UPDATE_HEALTH, (const char*) &s, sizeof(s_net_update_health), receiver);
}

