#include "level_sv.h"

level_sv::level_sv(const char *filename) : level(filename)
{
}

// TODO: destructor
level_sv::~level_sv()
{

}

void level_sv::spawn_starters()
{
	vec v;
	// TODO: create generator
	v.set(0, 0, border_ground);
	new box_sv(this, BOX_TYPE_GENERATOR, &v, 100);
	
	// create starting crates
	v.set(180, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(130, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(180, 130, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	
	v.set(-180, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(-130, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(-180, 130, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	
	v.set(-180, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(-130, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(-180, -130, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	
	v.set(180, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(130, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
	v.set(180, -130, border_ground);
	new box_sv(this, BOX_TYPE_WOODEN, &v, 100);
}

/*player_sv *level_sv::get_player(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_PLAYER) return (player_sv*)ac;
    return NULL;
}

npc_sv *level_sv::get_npc(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_NPC) return (npc_sv*)ac;
    return NULL;
}*/
