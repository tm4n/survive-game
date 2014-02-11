#include "collectible.h"


collectible::collectible(level *lvl, char acollectible_type, vec *pos)
	: actor(lvl, ACTOR_TYPE_COLLECTIBLE, pos, NULL)
{
	collectible_type = acollectible_type;
	faction = 0;
	
	passable = true;
}

collectible::collectible(level *lvl, uint actor_id, char acollectible_type, vec *pos)
	: actor(lvl, actor_id, ACTOR_TYPE_COLLECTIBLE, pos, NULL)
{
	collectible_type = acollectible_type;
	faction = 0;
	
	passable = true;
}


void collectible::movement(double)
{

}