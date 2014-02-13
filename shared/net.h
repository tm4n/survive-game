#ifndef __NET_H__
#define __NET_H__

#include "flist.h"
#include "defs.h"
#include "enet/enet.h"
#include "vec.h"
#include <cstdint>
#include <list>
#include <mutex>

////////////////////////////////////////////////
// package type defines
#define NET_VERSION 0

#define NET_CHAT 5

#define NET_GAME_STATE 8

#define NET_SYNC_SERVER 9
#define NET_SYNC_CLIENT 10

// SYNC also used for creation
#define NET_SYNC_PLAYER 11
#define NET_SYNC_BOX 12
#define NET_SYNC_COLLECTIBLE 13
#define NET_SYNC_NPC 14

#define NET_SYNC_FINISH 15

#define NET_REMOVE_ACTOR 19

#define NET_REQUEST_JOIN 20
#define NET_JOIN 21

#define NET_GAME_WAVE 22
#define NET_WAVE_WAIT_TIMER 23


#define NET_INPUT_KEYS 40
#define NET_TAKE 41
#define NET_SHOOT 42

#define NET_UPDATE_STATE 50
#define NET_UPDATE_TARGET 51
#define NET_UPDATE_POS 52
#define NET_UPDATE_ANG 53
#define NET_UPDATE_HEALTH 54
#define NET_UPDATE_WEAPON 55
#define NET_UPDATE_NPC_ORDERS 56

////////////////////////////////////////////////
// syncronization data

// align at 4
#pragma pack(push)
#pragma pack(4)

struct s_net_version
{
    uint32_t version;
};

struct s_net_sync_server
{
    int8_t mapfile[32];
    //...
};

// s_net_sync_client not there

struct s_net_sync_player
{
	uint32_t actor_id;
	vec pos;
	vec ang;
	float health;
    int8_t name[32];
	int32_t weapon;
	int32_t input;
	uint32_t object_taken;
};

struct s_net_sync_box
{
	uint32_t actor_id;
	uint32_t box_type;
	vec pos;
	float health;
};

struct s_net_sync_collectible
{
	uint32_t actor_id;
	uint32_t collectible_type;
	vec pos;
};

struct s_net_sync_npc
{
	uint32_t actor_id;
	uint32_t npc_type;
	vec pos;
	vec angle;
	float health;
	int target;
};

struct s_net_remove_actor
{
	uint32_t actor_id;
};

struct s_net_join
{
	uint32_t own_actor_id;
};

struct s_net_game_wave
{
	int32_t game_wave;
};

struct s_net_wave_wait_timer
{
	int32_t wave_wait_timer;
};

struct s_net_game_state
{
	int32_t state;
};

// value updates

struct s_net_input_keys
{
	uint32_t actor_id;
	int32_t input;
};

struct s_net_take
{
	uint32_t actor_id;
	int32_t taken_id;
};

struct s_net_shoot
{
	uint32_t actor_id;
	vec shoot_dir;
	int32_t rnd_seed;
};

struct s_net_update_state
{
    uint32_t actor_id;
    uint32_t state;
};

struct s_net_update_target
{
    uint32_t actor_id;
    uint32_t target;
};

struct s_net_update_pos
{
    uint32_t actor_id;
    vec pos;
};

struct s_net_update_ang
{
    uint32_t actor_id;
    float ang;
    float ang_interp_dir;
};

struct s_net_update_health
{
    uint32_t actor_id;
    float health;
};

struct s_net_update_npc_orders
{
    uint32_t actor_id;
    int npc_orders;
};

#pragma pack(pop)

extern bool enet_initialized;

class net
{
public:
	net();
	net(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);
	virtual ~net();
	
	bool local_only;
	bool is_server;
	
	ENetHost *eHost;
	
	ENetHost *host_create(const ENetAddress *address, size_t peerCount, size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth);
	void host_destroy();
	int host_service (ENetEvent *event, enet_uint32 timeout);
	void host_connect (const ENetAddress *address, size_t channelCount, enet_uint32 data);
	
	// networking functions
	int send_input_keys(uint actor_id, int input, ENetPeer *);
	int send_take(uint actor_id, int taken_id, ENetPeer *);
	int send_update_ang(uint actor_id, float ang, float ang_interp_dir, ENetPeer *);
	int send_update_pos(uint actor_id, vec *pos, ENetPeer *receiver);

	int send_event(uint16_t evtype, const char *data, uint32_t size, ENetPeer *);
	int broadcast_event(uint16_t evtype, const char *data, uint32_t size);
	int broadcast_event_except(uint16_t evtype, const char *data, uint32_t size, ENetPeer *expeer);
	
protected:
	bool net_local_only;
	std::list<ENetPacket*> *in_queue;
	std::list<ENetPacket*> *out_queue;
	std::mutex *mutex_in_queue;
	std::mutex *mutex_out_queue;
	
	ENetPeer *local_peer;
private:
	
};


////////////////////////////////////////////////
// network helper functions, both client and server


#endif
