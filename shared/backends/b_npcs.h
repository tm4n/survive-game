#ifndef __B_NPC_H__
#define __B_NPC_H__

#include "defs.h"
#include <iostream>
#include <string>

#define B_NPCS_ENTRIES 500

/*#define B_NPCS_INTERACT_TYPE_NONE 0
#define B_NPCS_INTERACT_TYPE_MERCHANT 1*/

struct s_npcs{
	std::string bfile, bname, binfo;

	int bfaction;
	float bmax_health;
	float bmove_speed;
	int blevel;
	float batt_range;
	float batt_speed;
	float batt_dmg_min;
	float batt_dmg_max;
	float batt_cooldown;
	float bbounty_xp;
	float bbounty_gold_probability;
	int bbounty_gold_min, bbounty_gold_max;

	int bdropset_id;
	int bdialogue_casual_id;
};

class b_npcs
{
public:
	s_npcs npcs[B_NPCS_ENTRIES];

	b_npcs(const char *);

	s_npcs *at(uint id);

private:
	void err(const char *, int);
};


// globally available instance
extern b_npcs *b_np;

#endif
