#ifndef __WEAPONMGR_H__
#define __WEAPONMGR_H__

#include "backends/b_weapons.h"
#include "level.h"

#define WEAPON_ENTRIES 10

class weaponmgr
{
public:

	weaponmgr(level *, int* curr_weapon);

	int get_curr_ammo();
	int get_curr_magazin();

	int pickups;

	int *curr_weapon;

	bool wp_ready;
	float wp_cooldown;
	int wp_reloading;

	short ammo[WEAPON_ENTRIES];
	short magazin[WEAPON_ENTRIES];

protected:
	level* lvl;

};

#endif