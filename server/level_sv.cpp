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
	// create starting crates
	vec v(0, 0, 0);
	new box_sv(this, &v, 100);
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
