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
