#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "defs.h"
#include "flist.h"
#include <list>
class actor;
class projectile;


class level
{
public:
    char filename[256];
    
    float border_ground;
    float border_height;
    float border_max;
    float border_min;
    
    float level_size;
    float level_ground;

	// actors in level
	flist<actor> actorlist;
	flist<projectile> projectilelist;

	level(const char *);
	virtual ~level();

    void get_actors_within(std::list<uint> *res, vec *pos, double range);
    int get_generator();
	bool trace(vec &from, vec &to, vec *hitpos, int *actor_hit, int actor_ignore1 = -1, int actor_ignore2 = -1);
};


#endif
