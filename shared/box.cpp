#include "box.h"

box::box(level *lvl, vec *pos, uint ahealth)
	: actor(lvl, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
}


box::box(level *lvl, uint actor_id, vec *pos, uint ahealth)
	: actor(lvl, actor_id, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
}
