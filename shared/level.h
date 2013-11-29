#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "defs.h"
#include "flist.h"
#include "actor.h"
#include <list>

class actor;




class level
{
public:
    char filename[256];

	// actors in level
	flist<actor> actorlist;
    uint actorlist_fixed; // the number of fixed actors that never change

	level(const char *);
	virtual ~level();

    void get_actors_within(std::list<uint> *res, vec *pos, double range);
};


#endif
