#include "weaponmgr_sv.h"

#include "helper.h"
#include "net_sv.h"
#include "npc_sv.h"
#include "scoremgr.h"
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
		ammo[weapon_id] = wdata->ammo_size;
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

	if (abs(shoot_origin.dist(&shoot_dir) - wdata->range) > 250) {log(LOG_ERROR, "WARNING: Possible cheating attempt on shoot target!"); return;}
	// TODO: create a flexible way to check cooldown

	if (magazin[*curr_weapon] <= 0) {log(LOG_ERROR, "WARNING: Possible cheating attempt, no ammo on client!"); return;}

	// reduce ammo (if not unlimited)
	if (wdata->ammo_size != -2) magazin[*curr_weapon] -= 1;

	int32_t seed = rand();
	random_seed(seed);
	
	log (LOG_DEBUG, "Starting shoot!");

	while (shoot_nums > 0)
	{
		shoot_nums--;

		shoot_target.set(&shoot_dir);
		shoot_target.x += (random_range(10)-5.f) * 10.f * wdata->accuracy;
		shoot_target.y += (random_range(10)-5.f) * 10.f * wdata->accuracy;
		shoot_target.z += (random_range(10)-5.f) * 10.f * wdata->accuracy;

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
				if (ac->type == ACTOR_TYPE_NPC && ac->faction == 2) // hit a zombie
				{
					npc_sv *np = (npc_sv *)ac;
					if (np->health > 0)
					{
						np->health -= wdata->damage;
						
						scoremgr::add_points(playerpeer, (uint)wdata->damage);
						if (ac->health <= 0) {ac->health = 0; scoremgr::add_points(playerpeer, b_npcs::instance()->at(np->npc_type)->bounty);}
						net_server->broadcast_update_health(ac->id, ac->health);
					}
				}
			}
		}
	
	}

	scoremgr::update_points(playerpeer);

	// TODO: a way to use cooldown on server
	//wp_cooldown = 10.f;
	
	// send including random seed
	net_server->broadcast_shoot(player_id, &shoot_dir, seed);
}

void weaponmgr_sv::wp_switch_impl(int num)
{
	log(LOG_DEBUG, "Server wp switch received");
	if (wp_cooldown > 0.f && wp_reloading == 0) {return;}
	if (num < 0 || num > WEAPON_ENTRIES) {return;}
	if (!(pickups & (1<<num)) || *curr_weapon == num) {return;}
	
	wp_ready = false;
	wp_reloading = 0; // cancel reloading
	
	wp_switching = num;
	wp_cooldown += 10.f;
	
	// send change back to client for animation
	net_server->send_change_weapon(player_id, num, playerpeer);
	log(LOG_DEBUG, "Server wp switch send out");
}

void weaponmgr_sv::wp_reload_impl()
{

	wp_ready = false;
	wp_cooldown += 10.f;
	wp_reloading = 1;
	
	net_server->broadcast_reload(player_id);
}

void weaponmgr_sv::cancel_reload()
{
	if (wp_reloading != 0)
	{
		wp_reloading = 0;
		wp_ready = true;
		wp_cooldown = 0.f;
	}
}


void weaponmgr_sv::frame(float time_frame)
{
	if (wp_switching != 0)
	{
		if (wp_cooldown > 0.f)  wp_cooldown -= time_frame;
		else
		{
			*curr_weapon = wp_switching;
			wp_switching = 0;
	
			net_server->broadcast_update_curr_weapon(player_id, *curr_weapon);
	
			wp_ready = true;
		}
	}

	if (wp_reloading != 0)
	{
		if (wp_cooldown > 0.f)  wp_cooldown -= b_weapons::instance()->at(*curr_weapon)->reloadspeed *time_frame;
		else
		{
			int num_ammo = ammo[*curr_weapon];
			int num_fired = b_weapons::instance()->at(*curr_weapon)->magazin_size - magazin[*curr_weapon];
		
			if (b_weapons::instance()->at(*curr_weapon)->ammo_size > 0)
			{
				if (num_ammo < num_fired)
				{
					// ammo has run out
					ammo[*curr_weapon] = 0;
					magazin[*curr_weapon] += num_ammo;
				}
				else
				{
					// enough ammo there
					ammo[*curr_weapon] = num_ammo - num_fired;
					magazin[*curr_weapon] += num_fired;
				}
			}
			else
			{
				// weapon has unlimited ammo
				magazin[*curr_weapon] = b_weapons::instance()->at(*curr_weapon)->magazin_size;
			}

			wp_ready = true;
			wp_reloading = 0;
		
			net_server->send_update_ammo_magazin(player_id, *curr_weapon, ammo[*curr_weapon], magazin[*curr_weapon], playerpeer);
		}
	}
}
