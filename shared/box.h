#ifndef __BOX_H__
#define __BOX_H__

#include "actor.h"

#define BOX_TYPE_GENERATOR 1
#define BOX_TYPE_WOODEN 2
#define BOX_TYPE_STEEL 3
#define BOX_TYPE_TURRET 4


class box : public actor
{

public:
	box(level *, char box_type, vec *, uint health);
	box(level *, uint actor_id, char box_type, vec *, uint health);
	
	char box_type;

};



#endif // __BOX_H__
