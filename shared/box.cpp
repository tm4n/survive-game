#include "box.h"
#include "helper.h"

box::box(level *lvl, char abox_type, vec *pos, float ahealth)
	: actor(lvl, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
	box_type = abox_type;
	state = BOX_STATE_PARACHUTING;
	gravity = 0.f;
	taker_id = -1;
	take_animation = -1.f;
	faction = 0;
	
	bb_max.x = 21.6f;
	bb_max.y = 21.6f;
	bb_max.z = 43.2f;
	bb_min.x = -21.6f;
	bb_min.y = -21.6f;
	bb_min.z = 0.f;
}


box::box(level *lvl, uint actor_id, char abox_type, vec *pos, float ahealth)
	: actor(lvl, actor_id, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
	box_type = abox_type;
	if (position.z > lvl->level_ground) state = BOX_STATE_PARACHUTING; else state = BOX_STATE_DEFAULT;
	gravity = 0.f;
	taker_id = -1;
	take_animation = -1.f;
	faction = 0;

	bb_max.x = 21.6f;
	bb_max.y = 21.6f;
	bb_max.z = 43.2f;
	bb_min.x = -21.6f;
	bb_min.y = -21.6f;
	bb_min.z = 0.f;
}

void box::movement(double time_delta)
{
	if (state == BOX_STATE_TAKEN)
	{
		// is moved by carrying entity
		passable = true;
		
		// get who took me
		actor *ac = lvl->actorlist.at(taker_id);
		if (ac != NULL)
		{		
			if (take_animation == -1.f) // just picked up
			{
				take_animation = 100.f;
				pickup_place.set(&position);
				pickup_place.x -= ac->position.x;
				pickup_place.y -= ac->position.y;
				pickup_place.z -= ac->position.z + ac->bb_max.z + 7.f;
			}
			else
			{
				take_animation -= (float)time_delta * 15.f;
				take_animation = std::max(take_animation, 0.f);
			}
			
			// move crate on top of playe
			position.x = ac->position.x + (pickup_place.x*(std::min(take_animation, 50.f)/50.f));
			position.y = ac->position.y + (pickup_place.y*(std::min(take_animation, 50.f)/50.f));
			position.z = (ac->position.z + ac->bb_max.z + 7.f) + ((pickup_place.z*(std::max(take_animation-25.f, 0.f)))/75.f);
		}
		else {log(LOG_ERROR, "invalid actor is registered as taker of a box"); state = BOX_STATE_DEFAULT;}
		
	}
	else
	{
		take_animation = -1;
		passable = false;
		
		// gravity
		gravity -= actor_gravity*(float)time_delta;
			
		if (state == BOX_STATE_PARACHUTING) gravity = clamp(gravity, -8.f, 8.f);
		else gravity = clamp (gravity, -30.f, 30.f);
			
		vec v(0.f, 0.f, gravity*(float)time_delta);
		float result = move_rel_col(&v);
		if (result <= 0.f)
		{
			gravity = 0.f;
			if (state == BOX_STATE_PARACHUTING)
			{
				state = BOX_STATE_DEFAULT;
			}
		}
	}
}
