#include "level.h"

#include "string.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "helper.h"
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

}

// Destructor
level::~level()
{
	delete[] tiles;
	delete[] props;
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
