#include "box.h"

box::box(level *lvl, char abox_type, vec *pos, uint ahealth)
	: actor(lvl, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
	box_type = abox_type;
}


box::box(level *lvl, uint actor_id, char abox_type, vec *pos, uint ahealth)
	: actor(lvl, actor_id, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
	box_type = abox_type;
}
