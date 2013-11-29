#include "player.h"
#include "backends/b_classes.h"

#include "string.h"

player::player(level *lvl, vec *pos, vec *pan,
               uint class_id,
               const char *name,
               uint level,
               uint strength,
               uint agility,
               uint intelligence,
               uint points_remain,
               uint xp,
               uint gold,
               double respawn_timer)

               : actor(lvl, ACTOR_TYPE_PLAYER, pos, pan)
{


    // set values
    this->class_id = class_id;
    this->faction = 1;

    strncpy(this->name, name, 32);
    this->name[31] = '\0';

    this->chlevel = level;

    this->strength = strength;
    this->agility = agility;
    this->intelligence = intelligence;

    this->points_remain = points_remain;

    this->xp = xp;
    this->gold = gold;
    this->bounty_xp = 0;

    this->respawn_timer = respawn_timer;

	for (int i = 0; i < PRJ_INV_EQUIPMENT; i++) equip[i] = 0;

    // set calculated values
    recalc_values();
	this->health = max_health;
	this->mana = max_mana;
}


player::player(level *lvl,
			   uint actor_id,
               uint class_id,
               const char *name)

               : actor(lvl, actor_id, ACTOR_TYPE_PLAYER)
{


    // set values
    this->class_id = class_id;
    this->faction = 1;

    strncpy(this->name, name, 32);
    this->name[31] = '\0';

    this->chlevel = 0;

    this->strength = 0;
    this->agility = 0;
    this->intelligence = 0;

    this->points_remain = 0;

    this->xp = 0;
	this->gold = 0;
    this->bounty_xp = 0;

	for (int i = 0; i < PRJ_INV_EQUIPMENT; i++) equip[i] = 0;
}




player::~player()
{
    //dtor
}

////////////////////////////////////////////////////
// calculate values


float player::calc_max_health()
{
    float hp = b_cl->at(class_id)->bstart_health + b_cl->at(class_id)->badd_health + 3.f*strength;

    if (inv != NULL) hp += ((inventory*)inv)->get_health_bonus();

    return hp;

}

float player::calc_max_mana()
{
    float ma = 10.f + intelligence*5.f + chlevel*3.f;

    if (inv != NULL) ma += ((inventory*)inv)->get_mana_bonus();

    return ma;

}

float player::calc_max_damage()
{
	float dmg = 2 + strength*0.1f;

	if (inv != NULL) dmg += ((inventory*)inv)->get_max_damage_bonus();

	return dmg;
}

float player::calc_min_damage()
{
	float dmg = 1 + strength*0.1f;

	if (inv != NULL) dmg += ((inventory*)inv)->get_min_damage_bonus();

	return dmg;
}

float player::calc_att_range()
{
    if (inv != NULL) return ((inventory*)inv)->get_wp_range();

    return 90;

}

float player::calc_att_speed()
{
	// TODO:
    if (inv != NULL)
    {
        float wp_speed = ((inventory*)inv)->get_wp_speed();
        if (wp_speed > 0) return wp_speed;
    }

    return 12;
}

float player::calc_att_cooldown()
{

    return 10; //TODO
}

float player::calc_defence()
{
    if (inv != NULL) return ((inventory*)inv)->get_defence_bonus();

    return 0;
}

float player::calc_move_speed()
{
    float speed = 7 + agility*0.05f;

    if (inv != NULL) speed += ((inventory*)inv)->get_move_speed_bonus();
    if (buffl != NULL) speed *= buffl->get_speed_modifier();

    return speed;
}

uint player::calc_next_xp()
{
    return 25u*chlevel*chlevel;
}


// set ALL the values
void player::recalc_values()
{
    max_health = calc_max_health();
    max_mana = calc_max_mana();

    att_dmg_max = calc_max_damage();
    att_dmg_min = calc_min_damage();
    att_range = calc_att_range();
    att_speed = calc_att_speed();
    att_cooldown = calc_att_cooldown();

    defence = calc_defence();
    move_speed = calc_move_speed();
}

void player::skill_cooldown_frame(double time_delta)
{
	if (skills != NULL)
	{
		for (uint i = 0; i < skills->size; i++)
		{
			skill *s = skills->at(i);
			if (s != NULL)
			{
				if (s->cooldown > 0)
				{
					s->cooldown -= (float)time_delta;
				}
			}
		}
	}
}

