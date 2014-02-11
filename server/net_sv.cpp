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


int net_sv::num_connected_clients()
{
	if (local_only)
	{
		return 1; // if singleplayer, there is always a player connected
	}
	else
	{
		int ct = 0;
		for (uint i = 0; i < eHost->peerCount; i++)
		{
			ENetPeer *p = &eHost->peers[i];
			if (eHost->peers[i].state == ENET_PEER_STATE_CONNECTED) ct++;
		}
		return ct;
	}
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

int net_sv::send_sync_box(uint actor_id, char box_type, vec *pos, float health, ENetPeer *receiver)
{
    s_net_sync_box s;

    s.actor_id = actor_id;
	s.box_type = box_type;
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

int net_sv::send_sync_collectible(uint actor_id, char collectible_type, vec *pos, ENetPeer *receiver)
{
    s_net_sync_collectible s;

    s.actor_id = actor_id;
	s.collectible_type = collectible_type;
    s.pos.set(pos);

    printf("net_sync_collectible mit actor_id=%u\n", actor_id);

    return send_event(NET_SYNC_COLLECTIBLE, (const char *)&s, sizeof(s_net_sync_collectible), receiver);

}


int net_sv::broadcast_sync_collectible(uint actor_id, char collectible_type, vec *pos)
{
    // TODO: broadcast only to synchronized players?
    s_net_sync_collectible s;

    s.actor_id = actor_id;
	s.collectible_type = collectible_type;
    s.pos.set(pos);

    printf("net_sync_collectible broadcast mit actor_id=%u\n", actor_id);

    return broadcast_event(NET_SYNC_COLLECTIBLE, (const char *)&s, sizeof(s_net_sync_collectible));

}

int net_sv::send_sync_npc(uint actor_id, int npc_type, vec *pos, vec *ang, float health, int target, ENetPeer *receiver)
{
	s_net_sync_npc s;

    s.actor_id = actor_id;
    s.npc_type = npc_type;
    s.pos.set(pos);
	s.angle.set(ang);
    s.health = health;
	s.target = target;

    printf("net_sync_npc mit actor_id=%u\n", actor_id);

    return send_event(NET_SYNC_NPC, (const char *)&s, sizeof(s_net_sync_npc), receiver);
}
int net_sv::broadcast_sync_npc(uint actor_id, int npc_type, vec *pos, vec *ang, float health, int target)
{
	// TODO: broadcast only to synchronized players?
    s_net_sync_npc s;

    s.actor_id = actor_id;
    s.npc_type = npc_type;
    s.pos.set(pos);
	s.angle.set(ang);
    s.health = health;
	s.target = target;

    printf("net_sync_npc broadcast mit actor_id=%u\n", actor_id);

    return broadcast_event(NET_SYNC_NPC, (const char *)&s, sizeof(s_net_sync_npc));
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

int net_sv::broadcast_game_wave(int game_wave)
{
    s_net_game_wave s;

	s.game_wave = game_wave;

    return broadcast_event(NET_GAME_WAVE, (const char *)&s, sizeof(s_net_game_wave));
}

int net_sv::broadcast_wave_wait_timer(int wave_wait_timer)
{
    s_net_wave_wait_timer s;

	s.wave_wait_timer = wave_wait_timer;

    return broadcast_event(NET_WAVE_WAIT_TIMER, (const char *)&s, sizeof(s_net_wave_wait_timer));
}

int net_sv::broadcast_game_state(int state)
{
    s_net_game_state s;

	s.state = state;

    return broadcast_event(NET_GAME_STATE, (const char *)&s, sizeof(s_net_game_state));
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

int net_sv::broadcast_update_pos_except(uint actor_id, vec *v, ENetPeer *except)
{
	s_net_update_pos s;

	s.actor_id = actor_id;
	s.pos.set(v);

    return broadcast_event_except(NET_UPDATE_POS, (const char*) &s, sizeof(s_net_update_pos), except);
}

int net_sv::broadcast_update_ang_except(uint actor_id, float ang, float ang_interp_dir, ENetPeer *except)
{
	s_net_update_ang s;

	s.actor_id = actor_id;
	s.ang = ang;
	s.ang_interp_dir = ang_interp_dir;

    return broadcast_event_except(NET_UPDATE_ANG, (const char*) &s, sizeof(s_net_update_ang), except);
}

int net_sv::broadcast_update_target(uint actor_id, uint target)
{
	s_net_update_target s;

    s.actor_id = actor_id;
    s.target = target;
	
	return broadcast_event(NET_UPDATE_TARGET, (const char *)&s, sizeof(s_net_update_target));
}

int net_sv::broadcast_update_npc_orders(uint actor_id, int npc_orders)
{
	s_net_update_npc_orders s;

    s.actor_id = actor_id;
    s.npc_orders = npc_orders;
	
	return broadcast_event(NET_UPDATE_NPC_ORDERS, (const char *)&s, sizeof(s_net_update_npc_orders));
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

int net_sv::broadcast_update_health(uint actor_id, float health)
{
    s_net_update_health s;

    s.actor_id = actor_id;
    s.health = health;

    return broadcast_event(NET_UPDATE_HEALTH, (const char *)&s, sizeof(s_net_update_health));
}

