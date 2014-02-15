#include "weaponmgr.h"

weaponmgr::weaponmgr(level *lvl, int *curr_weapon)
{
	this->lvl = lvl;

	this->pickups = 0;
	this->curr_weapon = curr_weapon;

	this->wp_ready = 0.f;
	this->wp_cooldown = 0.f;
	this->wp_reloading = 0.f;

	for (int i = 0; i < WEAPON_ENTRIES; i++)
	{
		ammo[i] = 0;
		magazin[i] = 0;
	}
}


int weaponmgr::get_curr_ammo()
{
	if (*curr_weapon < 0 && *curr_weapon >= WEAPON_ENTRIES) return 0;

	return ammo[*curr_weapon];
}

int weaponmgr::get_curr_magazin()
{
	if (*curr_weapon < 0 && *curr_weapon >= WEAPON_ENTRIES) return 0;

	return magazin[*curr_weapon];
}