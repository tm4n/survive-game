#include "collectible_sv.h"
#include "net_sv.h"
#include <sstream>

collectible_sv::collectible_sv(level_sv *lvl_sv, char acollectible_type, vec *pos)
	: collectible(lvl_sv, collectible_type, pos)
{
	this->lvl_sv = lvl_sv;
	
	std::ostringstream s;
    s << " created collectible, actor_id = " << id <<
		", type=\"" << collectible_type << "\"";

    log(LOG_DEBUG_VERBOSE, s.str().c_str());
    
    net_server->broadcast_sync_collectible(id, collectible_type, pos);
}

collectible_sv::~collectible_sv()
{
	net_server->broadcast_remove_actor(id);
}



void collectible_sv::frame(double time_frame)
{
	movement(time_frame);
	
	// check if players are nearby
	std::list<uint> lst;
	lvl_sv->get_players_within(&lst, &position, 60.);
	
	for (uint id : lst)
	{
		player_sv *pl = lvl_sv->get_player(id);
		if (pl != NULL)
		{
			// do smth
		}
	}
}
