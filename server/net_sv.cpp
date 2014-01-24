#include "net_sv.h"
#include "vec.h"

#include "string.h" // for memcpy, strcpy


net_sv *net_server = NULL;

net_sv::net_sv()
 : net()
{
	
}

net_sv::net_sv(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue)
   : net(in_queue, mutex_in_queue, out_queue, mutex_out_queue)
{
	
}


/////////////////////////////////////////////////////////////////
// specialized send functions
// server

int net_sv::send_version(int version, ENetPeer *receiver)
{
    s_net_version s;

    s.version = version;

    return send_event(NET_VERSION, (const char *)&s, sizeof(s_net_version), receiver);
}


int net_sv::send_sync_server(const char *mapfile, ENetPeer *receiver)
{
    s_net_sync_server s;

    strncpy((char*)s.mapfile, mapfile, 32);
    s.mapfile[31] = '\0';

    return send_event(NET_SYNC_SERVER, (const char *)&s, sizeof(s_net_sync_server), receiver);
}

int net_sv::broadcast_chat(const char* msg, uint len)
{
	return broadcast_event(NET_CHAT, msg, len);
}


int net_sv::send_sync_player(uint actor_id, vec *pos, vec *ang, float health, const char *name, int weapon, int input, int object_taken, ENetPeer *receiver)
{
    s_net_sync_player s;

	s.actor_id = actor_id;
	s.pos.set(pos);
	s.ang.set(ang);
	strncpy((char*)s.name, name, 32);
	s.name[31] = '\0';
	s.weapon = weapon;
	s.input = input;
	s.object_taken = object_taken;

    printf("net_sync_player mit actor_id=%u, name=%s \n", actor_id, name);

    return send_event(NET_SYNC_PLAYER, (const char *)&s, sizeof(s_net_sync_player), receiver);

}

int net_sv::broadcast_sync_player(uint actor_id, vec *pos, vec *ang, float health, const char *name, int weapon, int input, int object_taken)
{
    // TODO: broadcast only to synchronized players?
    s_net_sync_player s;

    s.actor_id = actor_id;
	s.pos.set(pos);
	s.ang.set(ang);
	s.health = health;
	strncpy((char*)s.name, name, 32);
	s.name[31] = '\0';
	s.weapon = weapon;
	s.input = input;
	s.object_taken = object_taken;

    printf("net_sync_player_broadcast mit actor_id=%u, name=%s \n", actor_id,  name);

    return broadcast_event(NET_SYNC_PLAYER, (const char *)&s, sizeof(s_net_sync_player));
}

int net_sv::send_sync_box(uint actor_id, vec *pos, float health, ENetPeer *receiver)
{
    s_net_sync_box s;

    s.actor_id = actor_id;
    s.pos.set(pos);
    s.health = health;

    printf("net_sync_box mit actor_id=%u\n", actor_id);

    return send_event(NET_SYNC_BOX, (const char *)&s, sizeof(s_net_sync_box), receiver);

}

int net_sv::broadcast_sync_box(uint actor_id, char box_type, vec *pos, float health)
{
    // TODO: broadcast only to synchronized players?
    s_net_sync_box s;

    s.actor_id = actor_id;
    s.box_type = box_type;
    s.pos.set(pos);
    s.health = health;

    printf("net_sync_box broadcast mit actor_id=%u\n", actor_id);

    return broadcast_event(NET_SYNC_BOX, (const char *)&s, sizeof(s_net_sync_box));

}

int net_sv::broadcast_remove_actor(uint actor_id)
{
    // TODO: broadcast only to synchronized players?
    s_net_remove_actor s;

	s.actor_id = actor_id;

    printf("net_sync_remove_broadcast mit actor_id=%u\n", actor_id);

    return broadcast_event(NET_REMOVE_ACTOR, (const char *)&s, sizeof(s_net_remove_actor));
}

int net_sv::send_sync_finish(ENetPeer *receiver)
{
    printf("sending net_sync_finish\n");

    return send_event(NET_SYNC_FINISH, NULL, 0, receiver);
}

int net_sv::send_join(uint own_actor_id, ENetPeer *receiver)
{
	s_net_join s;
	
	s.own_actor_id = own_actor_id;
	
	printf("sending net_send_join\n");

    return send_event(NET_JOIN, (const char *)&s, sizeof(s_net_join), receiver);
}

int net_sv::broadcast_take(uint actor_id, int taken_id)
{
    s_net_take s;

	s.actor_id = actor_id;
	s.taken_id = taken_id;

    printf("broadcast_take mit actor_id=%u\n", actor_id);

    return broadcast_event(NET_TAKE, (const char *)&s, sizeof(s_net_take));
}

// update values
int net_sv::broadcast_update_pos(uint actor_id, vec *v)
{
	s_net_update_pos s;
	
	s.actor_id = actor_id;
	s.pos.set(v);
	
	return broadcast_event(NET_UPDATE_POS, (const char *)&s, sizeof(s_net_update_pos));
}

int net_sv::broadcast_update_ang_except(uint actor_id, float ang, float ang_interp_dir, ENetPeer *receiver)
{
	s_net_update_ang s;

	s.ang = ang;
	s.ang_interp_dir = ang_interp_dir;

    return broadcast_event_except(NET_UPDATE_ANG, (const char*) &s, sizeof(s_net_update_ang), receiver);
}

int net_sv::send_update_target(uint actor_id, uint target, ENetPeer *receiver)
{
    s_net_update_target s;

    s.actor_id = actor_id;
    s.target = target;

    return send_event(NET_UPDATE_TARGET, (const char*) &s, sizeof(s_net_update_target), receiver);
}

int net_sv::send_update_health(uint actor_id, float health, ENetPeer *receiver)
{
    s_net_update_health s;

    s.actor_id = actor_id;
    s.health = health;

    return send_event(NET_UPDATE_HEALTH, (const char*) &s, sizeof(s_net_update_health), receiver);
}

