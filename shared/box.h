#ifndef __BOX_H__
#define __BOX_H__

#include "actor.h"

#define BOX_TYPE_GENERATOR 1
#define BOX_TYPE_WOOD 2
#define BOX_TYPE_METAL 3
#define BOX_TYPE_TURRET 4

#define BOX_STATE_DEFAULT 0
#define BOX_STATE_PARACHUTING 1
#define BOX_STATE_TAKEN 2

class box : public actor
{

public:
	box(level *, char box_type, vec *, float health);
	box(level *, uint actor_id, char box_type, vec *, float health);
	
	void movement(double);

	int taker_id;
	char box_type;
	float gravity;
	float take_animation; // progress while animating box
	vec pickup_place; //  position difference where box was picked up
};



#endif // __BOX_H__
