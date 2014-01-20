#ifndef __NET_H__
#define __NET_H__

#include "defs.h"
#include "enet/enet.h"
#include "vec.h"
#include <cstdint>

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
#define NET_SYNC_CONTAINER 13
#define NET_SYNC_NPC 14

#define NET_SYNC_FINISH 15

#define NET_REMOVE_ACTOR 19

#define NET_REQUEST_JOIN 20
#define NET_JOIN 21


// #define NET_INPUT

#define NET_UPDATE_STATE 50
#define NET_UPDATE_TARGET 51
#define NET_UPDATE_POS 52
#define NET_UPDATE_HEALTH 53
#define NET_UPDATE_WEAPON 54


extern ENetHost *gEhost;

////////////////////////////////////////////////
// syncronization data

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
	int32_t state;
	int32_t input;
};

struct s_net_sync_box
{
	uint32_t actor_id;
	uint8_t box_type;
	vec pos;
	float health;
};

struct s_net_remove_actor
{
	uint32_t actor_id;
};

struct s_net_join
{
	uint32_t own_actor_id;
};

// value updates
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
    float x, y;
};

struct s_net_update_health
{
    uint actor_id;
    uint32_t health;
};

///////////////////////////////////////////////
// global variables
extern bool net_local_only;


////////////////////////////////////////////////
// network helper functions, both client and server

int net_send_event(uint16_t evtype, const char *data, uint32_t size, ENetPeer *);
int net_broadcast_event(uint16_t evtype, const char *data, uint32_t size, ENetHost *);

#endif
