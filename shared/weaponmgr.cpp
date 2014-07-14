#include "weaponmgr.h"

weaponmgr::weaponmgr(level *lvl, int *curr_weapon)
{
	this->lvl = lvl;

	this->curr_weapon = curr_weapon;

	this->wp_ready = true;
	this->wp_cooldown = 0.f;
	this->wp_reloading = 0;
	this->wp_switching = 0;

	for (int i = 0; i < WEAPON_ENTRIES; i++)
	{
		ammo[i] = 0;
		magazin[i] = 0;
	}
	
	// give colt by default
	*curr_weapon = 1;
	this->pickups = (1 << WP_COLT);
	this->magazin[WP_COLT] = 7;
	this->ammo[WP_COLT] = -1;
}


int weaponmgr::get_curr_ammo()
{
	if (*curr_weapon < 0 || *curr_weapon >= WEAPON_ENTRIES) {std::cout << "ERROR on getting curr ammo"; return -1;}

	return ammo[*curr_weapon];
}

int weaponmgr::get_curr_magazin()
{
	if (*curr_weapon < 0 || *curr_weapon >= WEAPON_ENTRIES) return -1;

	return magazin[*curr_weapon];
}
