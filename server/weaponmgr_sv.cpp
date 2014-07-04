#include "weaponmgr_sv.h"

#include "helper.h"
#include "net_sv.h"
#include "backends/b_weapons.h"
#include <sstream>

weaponmgr_sv::weaponmgr_sv(level *lvl, int *curr_weapon, ENetPeer *playerpeer, int player_id)
	: weaponmgr(lvl, curr_weapon)
{
	this->playerpeer = playerpeer;
	this->player_id = player_id;
}


void weaponmgr_sv::give_weapon(int weapon_id)
{
	bool send_update = false;
	
	s_weapons *wdata = b_weapons::instance()->at(weapon_id);

	if (weapon_id < 0 || weapon_id >= WEAPON_ENTRIES || wdata == NULL) {log(LOG_ERROR, "Tried to give invalid weapon number"); return;}
	
	// also give full magazin on first pickup
	if (!(pickups & (1<<weapon_id)))
	{
		pickups |= (1<<weapon_id);
		log(LOG_DEBUG, "Weapon given to player");
		
		magazin[weapon_id] = wdata->magazin_size;
		send_update = true;
	}
	
	// fill up ammo
	if (wdata->ammo_size > 0)
	{
		if (ammo[weapon_id] < wdata->ammo_size)
		{
			ammo[weapon_id] =  wdata->ammo_size;
			// send ammo update to client
			send_update = true;
		}
	}
	
	if (send_update) net_server->send_update_ammo_magazin(player_id, weapon_id, ammo[weapon_id], magazin[weapon_id], playerpeer);

	if (*curr_weapon == 0) *curr_weapon = weapon_id;
}

void weaponmgr_sv::shoot(vec &shoot_origin, vec &shoot_dir)
{
	vec shoot_target;
	
	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);

	int shoot_nums = wdata->bullets;

	if (abs(shoot_origin.dist(&shoot_dir) - wdata->range) > 500) {log(LOG_ERROR, "WARNING: Possible cheating attempt on shoot target!"); return;}
	
	int32_t seed = rand();
	srand(seed);

	while (shoot_nums > 0)
	{
		shoot_nums--;

		shoot_target.set(&shoot_dir);
		shoot_target.x += (random(10)-5.f) * 10.f * wdata->accuracy;
		shoot_target.y += (random(10)-5.f) * 10.f * wdata->accuracy;
		shoot_target.z += (random(10)-5.f) * 10.f * wdata->accuracy;

		/*std::ostringstream s;
		s << "Shooting between " << shoot_origin << " and " << shoot_target;
		log(LOG_DEBUG_VERBOSE, s.str().c_str());*/

		vec hitpos;
		int actor_hit = 0;
		if (lvl->trace(player_id, shoot_origin, shoot_target, &hitpos, &actor_hit))
		{
			if (actor_hit >= 0) // hit someone
			{
				actor *ac = lvl->actorlist.at(actor_hit);
				if (ac->faction == 2) // hit a zombie
				{
					if (ac->health > 0)
					{
						ac->health -= wdata->damage;
						//game_score[ent.owner-1] += wp_data[ent.weapon].damage;
						if (ac->health <= 0) {ac->health = 0;} //game_score[ent.owner-1] += npc_data[you.npc_type].bounty;}
						net_server->broadcast_update_health(ac->id, ac->health);
					}
				}
			}
		}
	
	}
	
	//net_server->broadcast_shoot();
	// send including random seed
	//enet_send_skills(enet_ent_globpointer(me), SK_SHOOT_DIR, SK_SHOOT_DIR+3, BROADCAST);
}