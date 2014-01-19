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
    char mapfile[32];
    //...
};

// s_net_sync_client not there

struct s_net_sync_player
{
	uint actor_id;
	vec pos;
	vec ang;
	float health;
    char name[32];
	int state;
	int input;
};

struct s_net_sync_box
{
	uint actor_id;
	char box_type;
	vec pos;
	float health;
};

struct s_net_remove_actor
{
	uint actor_id;
};

struct s_net_join
{
	uint own_actor_id;
};

// value updates
struct s_net_update_state
{
    uint actor_id;
    ushort state;
};

struct s_net_update_target
{
    uint actor_id;
    uint target;
};

struct s_net_update_pos
{
    uint actor_id;
    float x, y;
};

struct s_net_update_health
{
    uint actor_id;
    float health;
};

///////////////////////////////////////////////
// global variables
extern bool net_local_only;


////////////////////////////////////////////////
// network helper functions, both client and server

int net_send_event(short evtype, const char *data, int size, ENetPeer *);
int net_broadcast_event(short evtype, const char *data, int size, ENetHost *);

#endif
