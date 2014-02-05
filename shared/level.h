#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "defs.h"
#include "flist.h"
#include <list>
class actor;



class level
{
public:
    char filename[256];
    
    float border_ground;
    float border_height;
    float border_max;
    float border_min;

	// actors in level
	flist<actor> actorlist;

	level(const char *);
	virtual ~level();

    void get_actors_within(std::list<uint> *res, vec *pos, double range);
    int get_generator();
};


#endif
