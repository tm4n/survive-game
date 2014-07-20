#ifndef __SCOREMGR_H__
#define __SCOREMGR_H__

#include "defs.h"
#include "enet/enet.h"

class scoremgr
{
public:
	static void add_points(uint actor_id, int num_points);
	static void add_points(ENetPeer *peer, int num_points);
	static void update_points(uint actor_id);
	static void update_points(ENetPeer *peer);
	static void remove_points_death(uint actor_id);
	static void get_full_string(std::string *str);
	static void clear_all();
};

#endif