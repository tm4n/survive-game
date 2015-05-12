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
	death_timer = 0.f;


    std::ostringstream s;
    s << " created Player, actor_id = " << id <<
        ", name=\"" << name << "\"";

    log(LOG_DEBUG_VERBOSE, s.str().c_str());
    
    net_server->broadcast_sync_player(id, pos, ang, health, name, curr_weapon, input, object_taken);

	// wpmgr->give_weapon(WP_COLT); now automatically given

	// cheats below
	wpmgr->give_weapon(WP_WESSON);
	wpmgr->give_weapon(WP_CHAINSAW);
	wpmgr->give_weapon(WP_HKSL8);
	wpmgr->give_weapon(WP_SHOTGUN);
	wpmgr->give_weapon(WP_USAS12);
}

player_sv::~player_sv()
{
    log(LOG_DEBUG_VERBOSE, "player_sv: deleting player");

	net_server->broadcast_remove_actor(id);

	delete wpmgr;
}


void player_sv::frame(double time_delta)
{
	if (input & INPUT_SPRINT)
	{
		wpmgr->cancel_reload();
	}

	movement((float)time_delta);

	wpmgr->frame((float)time_delta);


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

	// Death
	if (health <= 0.f)
	{
		death_timer += (float)time_delta;

		if (death_timer > 2.f*16.f)
		{
			s_peer_data *d = net_server->get_peer_data_for_id(id);
			if (d != NULL)
			{
				d->respawn_timer = RESPAWN_TIME;
				d->clstate = 3;
				d->player_actor_id = 0;
			}

			if (object_taken >= 0)
			{
				box_sv *box = ((level_sv*)lvl)->get_box((uint)object_taken);
				if (box != NULL)
				{
					box->taker_id = -1;
					box->state = BOX_STATE_DEFAULT;
									
					net_server->broadcast_take(id, box->id);
					net_server->broadcast_update_pos(box->id, &box->position);

					object_taken = -1;
				}
			}

			delete this;
			return;
		}
	}
}
