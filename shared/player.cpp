#include "player.h"

#include "string.h"

player::player(level *, vec *pos, vec *ang, float ahealth,
               const char *name)

               : actor(lvl, ACTOR_TYPE_PLAYER, pos, ang)
{
	this->health = health;

    strncpy(this->name, name, 32);
    this->name[31] = '\0';
}


player::player(level *, 
			   uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int state, int input)

               : actor(lvl, actor_id, ACTOR_TYPE_PLAYER, pos, ang)
{
	this->health = health;

	this->state = state;
	this->input = input;

    strncpy(this->name, name, 32);
    this->name[31] = '\0';
}




player::~player()
{
    //dtor
}
