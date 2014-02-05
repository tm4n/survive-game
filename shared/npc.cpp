#include "npc.h"
#include <backends/b_npcs.h>

npc::npc(level* lvl, uint npc_type, vec *pos, vec *pan) : actor(lvl, ACTOR_TYPE_NPC, pos, pan)
{
    this->npc_type = npc_type;

    // put to ground
    this->position.z = lvl->border_ground;

    init_values();
}

npc::~npc()
{
    //dtor
}


void npc::init_values()
{
    // set values from backend

    /*this->max_health = b_np->npcs[npc_type].bmax_health;
    this->health = max_health;
    this->faction = b_np->npcs[npc_type].bfaction;
    this->move_speed = b_np->npcs[npc_type].bmove_speed;
    this->chlevel = b_np->npcs[npc_type].blevel;
    this->max_mana = 0;// TODO: mana?
    this->mana = this->max_mana;
    this->defence = 0;
    this->att_range = b_np->npcs[npc_type].batt_range;
    this->att_speed = b_np->npcs[npc_type].batt_speed;
    this->att_dmg_max = b_np->npcs[npc_type].batt_dmg_max;
    this->att_dmg_min = b_np->npcs[npc_type].batt_dmg_min;
    this->att_cooldown = b_np->npcs[npc_type].batt_cooldown;
    this->bounty_xp = b_np->npcs[npc_type].bbounty_xp;*/
}
