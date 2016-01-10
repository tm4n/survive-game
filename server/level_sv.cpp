#include "level_sv.h"

level_sv::level_sv(const char *filename) : level(filename)
{
	for (int i = 0; i < NUM_WPDROPS; i++) wpdrops[i] = false;
}

// TODO: destructor
level_sv::~level_sv()
{
	for (uint i = 0; i < actorlist.size; i++)
	{
		if (actorlist.at(i) != 0) delete actorlist.at(i);
	}
	//assert(actorlist.filled == 0);
	for (uint i = 0; i < projectilelist.size; i++)
	{
		if (projectilelist.at(i) != 0) delete projectilelist.at(i);
	}
	//assert(projectilelist.filled == 0);
}

player_sv *level_sv::get_player(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_PLAYER) return (player_sv*)ac;
    return NULL;
}

collectible_sv *level_sv::get_collectible(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_COLLECTIBLE) return (collectible_sv*)ac;
    return NULL;
}

box_sv *level_sv::get_box(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_BOX) return (box_sv*)ac;
    return NULL;
}


void level_sv::get_players_within(std::list<uint> *res, vec *pos, double range)
{
    for (uint i = 0; i < actorlist.size; i++)
    {
        actor *act = actorlist.at(i);
        if (act != NULL)
        {
            // check if within range
            if (act->type == ACTOR_TYPE_PLAYER)
            {
				if (act->position.dist(pos) <= range)
				{
					res->push_back(i);
				}
            }
        }
    }
}
