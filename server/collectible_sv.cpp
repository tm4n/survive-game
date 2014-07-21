#include "collectible_sv.h"
#include "net_sv.h"
#include <sstream>

collectible_sv::collectible_sv(level_sv *lvl_sv, char acollectible_type, vec *pos)
	: collectible(lvl_sv, acollectible_type, pos)
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
			if (collectible_type == COLLECTIBLE_TYPE_WP_COLT) pl->wpmgr->give_weapon(WP_COLT);
			if (collectible_type == COLLECTIBLE_TYPE_WP_CHAINSAW) pl->wpmgr->give_weapon(WP_CHAINSAW);
			if (collectible_type == COLLECTIBLE_TYPE_WP_WESSON) pl->wpmgr->give_weapon(WP_WESSON);
			if (collectible_type == COLLECTIBLE_TYPE_WP_HKSL8) pl->wpmgr->give_weapon(WP_HKSL8);
			if (collectible_type == COLLECTIBLE_TYPE_WP_SHOTGUN) pl->wpmgr->give_weapon(WP_SHOTGUN);
			if (collectible_type == COLLECTIBLE_TYPE_WP_USAS12) pl->wpmgr->give_weapon(WP_USAS12);
			

			if (collectible_type == COLLECTIBLE_TYPE_HEALTH)
			{
				if (pl->health < 100.f)
				{
					// give health
					pl->change_health(50.f);
					net_server->send_update_health(pl->id, pl->health, pl->owner);

					delete this;
					return;
				}
			}
		}
	}
}
