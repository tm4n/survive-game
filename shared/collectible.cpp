#include "collectible.h"


collectible::collectible(level *lvl, char acollectible_type, vec *pos)
	: actor(lvl, ACTOR_TYPE_COLLECTIBLE, pos, NULL)
{
	this->collectible_type = acollectible_type;
	this->faction = 0;
	
	passable = true;

	bb_max.x = 10.f;
	bb_max.y = 20.f;
	bb_max.z = 8.f;
	bb_min.x = -10.f;
	bb_min.y = -20.f;
	bb_min.z = -8.f;
	
	if (collectible_type < 20) state = COLLECTIBLE_STATE_PARACHUTE; else state = COLLECTIBLE_STATE_DEFAULT;
}

collectible::collectible(level *lvl, uint actor_id, char acollectible_type, vec *pos)
	: actor(lvl, actor_id, ACTOR_TYPE_COLLECTIBLE, pos, NULL)
{
	collectible_type = acollectible_type;
	faction = 0;
	
	passable = true;

	bb_max.x = 10.f;
	bb_max.y = 20.f;
	bb_max.z = 8.f;
	bb_min.x = -10.f;
	bb_min.y = -20.f;
	bb_min.z = -8.f;

	
	if (collectible_type < 20) state = COLLECTIBLE_STATE_PARACHUTE; else state = COLLECTIBLE_STATE_DEFAULT;
}


void collectible::movement(double frame_time)
{
	if (state == COLLECTIBLE_STATE_PARACHUTE)
	{
		position.z -= 8.f* (float)frame_time;
		
		if (position.z <= lvl->border_ground-bb_min.z)
		{
			position.z = lvl->border_ground-bb_min.z;
			state = COLLECTIBLE_STATE_DEFAULT;
		}
	}
}
