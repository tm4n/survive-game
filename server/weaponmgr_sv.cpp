#include "weaponmgr_sv.h"

#include "helper.h"
#include "net_sv.h"

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
}