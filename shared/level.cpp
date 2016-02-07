#include "level.h"

#include "string.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "helper.h"
#include "actor.h"
#include "box.h"
using namespace std;

// map access macro
#define mdim(i,j) (((i)*size)+(j))

///////////////////////////////////////////
// Constructor: create a level from file
level::level(const char *afilename)
{
    log(LOG_DEBUG, "Opening level:");
    log(LOG_DEBUG, afilename);

    strncpy(this->filename, afilename, 256);
    
	border_ground = -295.f; // -240
	border_height = 1400.f;
	border_max = 2000.f;
	border_min = -2000.f;
	
	level_size = 2000.f; // determains spawn position
	level_ground = -240.f;
}

// Destructor
level::~level()
{
}

int level::get_generator()
{
	for (uint i = 0; i < actorlist.size; i++)
	{
		actor *ac = actorlist.at(i);
		if (ac != NULL)
		{
			if (ac->type == ACTOR_TYPE_BOX)
			{
				box *b = (box*)ac;
				if (b->box_type == BOX_TYPE_GENERATOR) return i;
			}
		}
	}
	
	return -1;
}



void level::get_actors_within(std::list<uint> *res, vec *pos, double range)
{
    for (uint i = 0; i < actorlist.size; i++)
    {
        actor *act = actorlist.at(i);
        if (act != NULL)
        {
            // check if within range
            if ((act->position.x - pos->x)*(act->position.x - pos->x)  +  (act->position.y - pos->y)*(act->position.y - pos->y) <= range*range)
            {
                res->push_back(i);
            }

        }
    }
}

bool level::trace(vec &from, vec &to, vec *hitpos, int *actor_hit, int actor_ignore1, int actor_ignore2)
{
	vec col_move_world, col_move_border_min, col_move_border_max;
	
	// get difference
	col_move_world.x = to.x - from.x;
	col_move_world.y = to.y - from.y;
	col_move_world.z = to.z - from.z;
	
	col_move_border_min.set(border_min, border_min, border_ground);
	col_move_border_max.set(border_max, border_max, border_height);

	int steps = (int)(to.dist(&from)*1.5f);
	steps = clamp(steps, 5, 10000);
	vec curr_pos;
	for (int i = 0; i < steps; i++)
	{
		// calculate current pos
		curr_pos.x = from.x + col_move_world.x*((float)i/(float)steps);
		curr_pos.y = from.y + col_move_world.y*((float)i/(float)steps);
		curr_pos.z = from.z + col_move_world.z*((float)i/(float)steps);

		// check if pos out of world
		if ((curr_pos.x < col_move_border_min.x || curr_pos.x > col_move_border_max.x) // out of x plane
			|| (curr_pos.y < col_move_border_min.y || curr_pos.y > col_move_border_max.y) // out of y plane
			|| (curr_pos.z < col_move_border_min.z || curr_pos.z > col_move_border_max.z)) // out of z plane
		{
			//log(LOG_DEBUG_VERBOSE, "trace is out of world");
			// out of world
			if (hitpos) hitpos->set(&curr_pos);
			if (actor_hit) *actor_hit =  -1;

			return true;
		}

		// check if pos in in object/border
		for (uint i = 0; i < actorlist.size; i++)
		{
			actor *ac = actorlist.at(i);
			if (ac != NULL)
			{
				if (ac->health > 0.f && !ac->passable && (int)ac->id != actor_ignore1 && (int)ac->id != actor_ignore2)
				{	
					if ((curr_pos.x < ac->position.x + ac->bb_max.x && curr_pos.x > ac->position.x + ac->bb_min.x) // collision in x plane
						&& (curr_pos.y < ac->position.y + ac->bb_max.y && curr_pos.y > ac->position.y + ac->bb_min.y) // collision in y plane
						&& (curr_pos.z < ac->position.z + ac->bb_max.z && curr_pos.z > ac->position.z + ac->bb_min.z)) // collision in z plane
					{
						// hit an entity
						if (hitpos) hitpos->set(&curr_pos);
						if (actor_hit) *actor_hit =  i;

						return true;
					}

				}
			}
		}

	}

	return false;
}