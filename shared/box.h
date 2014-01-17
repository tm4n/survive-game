#ifndef __BOX_H__
#define __BOX_H__

#include "actor.h"


class box : public actor
{

public:
	box(level *, vec *, uint health);
	box(level *, uint actor_id, vec *, uint health);

};



#endif // __BOX_H__
