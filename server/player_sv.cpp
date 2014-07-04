#include "player_sv.h"
#include "net_sv.h"

#include <sstream>



player_sv::player_sv(level *lvl, vec *pos, vec *ang, float health,
               const char *name, ENetPeer *owner)
               : player(lvl, pos, ang, health, name)
{

    this->owner = owner;
	this->wpmgr = new weaponmgr_sv(lvl, &curr_weapon, owner, id);
    
    ang_count = 999;
	send_pos_timer = 0.f;


    std::ostringstream s;
    s << " created Player, actor_id = " << id <<
        ", name=\"" << name << "\"";

    log(LOG_DEBUG_VERBOSE, s.str().c_str());
    
    net_server->broadcast_sync_player(id, pos, ang, health, name, curr_weapon, input, object_taken);

	wpmgr->give_weapon(WP_COLT);
}

player_sv::~player_sv()
{
    log(LOG_DEBUG_VERBOSE, "player_sv: deleting player");
	delete wpmgr;
}


void player_sv::frame(double time_delta)
{
	movement((float)time_delta);


	// send pos only to other players
	send_pos_timer += (float)time_delta;
	if (send_pos_timer >= SV_SEND_PLAYER_RATE)
	{
		send_pos_timer -= SV_SEND_PLAYER_RATE;
		if (position.dist(&last_position) > 0.01f)
		{
			last_position.set(&position);
			net_server->broadcast_update_pos_except(id, &position, owner);
		}
	}
}
