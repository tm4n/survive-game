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

class net_sv : public net
{
public:
	
	net_sv();
	net_sv(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);
	
	///////////////////////////////////////////////
	// special send functions for server

	int send_version(int version, ENetPeer *receiver);
	int send_sync_server(const char *mapfile, ENetPeer *receiver);

	int broadcast_chat(const char* msg, uint len);

	int send_sync_player(uint actor_id, vec *pos, vec *ang, float health, const char *name, int weapon, int input, int object_taken, ENetPeer *receiver);
	int broadcast_sync_player(uint actor_id, vec *pos, vec *ang, float health, const char *name, int weapon, int input, int object_taken);

	int send_sync_box(uint actor_id, char box_type, vec *pos, float health, ENetPeer *receiver);
	int broadcast_sync_box(uint actor_id, char box_type, vec *pos, float health);

	int send_sync_npc(uint actor_id, int npc_type, vec *pos, vec *ang, float health, int target, ENetPeer *receiver);
	int broadcast_sync_npc(uint actor_id, int npc_type, vec *pos, vec *ang, float health, int target);


	int send_sync_finish(ENetPeer *receiver);

	int broadcast_remove_actor(uint actor_id);

	int send_join(uint own_actor_id, ENetPeer *receiver);
	
	int broadcast_take(uint actor_id, int taken_id);

	int broadcast_update_ang_except(uint actor_id, float ang, float ang_interp_dir, ENetPeer *);
	int broadcast_update_pos(uint actor_id, vec *v);
	int broadcast_update_pos_except(uint actor_id, vec *v, ENetPeer *);
	int send_update_target(uint actor_id, uint target, ENetPeer *receiver);
	int broadcast_update_target(uint actor_id, uint target);
	int broadcast_update_npc_orders(uint actor_id, int npc_orders);
	int send_update_health(uint actor_id, float health, ENetPeer *receiver);
	int broadcast_update_health(uint actor_id, float health);
};


extern net_sv *net_server;

#endif // __NET_SV_H__
