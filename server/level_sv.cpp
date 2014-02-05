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
	vec v, t;
	// TODO: create generator
	v.set(0, 0, border_ground);
	new box_sv(this, BOX_TYPE_GENERATOR, &v);
	
	// create starting crates
	v.set(180, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(130, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(180, 130, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	
	v.set(-180, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(-130, 180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(-180, 130, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	
	v.set(-180, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(-130, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(-180, -130, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	
	v.set(180, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(130, -180, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);
	v.set(180, -130, border_ground);
	new box_sv(this, BOX_TYPE_WOOD, &v);

	v.set(400, 0, border_ground+100.f);
	new npc_sv(this, NPC_MUMMY, &v, &t);

	v.set(400, 300, 0.f);
	new npc_sv(this, NPC_HARPY, &v, &t);
}

player_sv *level_sv::get_player(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_PLAYER) return (player_sv*)ac;
    return NULL;
}

box_sv *level_sv::get_box(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_BOX) return (box_sv*)ac;
    return NULL;
}


