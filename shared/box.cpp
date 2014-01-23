#include "box.h"
#include "helper.h"

box::box(level *lvl, char abox_type, vec *pos, float ahealth)
	: actor(lvl, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
	box_type = abox_type;
	state = BOX_STATE_PARACHUTING;
	gravity = 0.f;
}


box::box(level *lvl, uint actor_id, char abox_type, vec *pos, float ahealth)
	: actor(lvl, actor_id, ACTOR_TYPE_BOX, pos, NULL)
{
	health = ahealth;
	box_type = abox_type;
	state = BOX_STATE_PARACHUTING;
	gravity = 0.f;

	bb_max.x = 21.6f;
	bb_max.y = 21.6f;
	bb_max.z = 43-2.f;
	bb_min.x = -21.6f;
	bb_min.y = -21.6f;
	bb_min.z = 0.f;
}

void box::movement(double time_delta)
{
	if (state == BOX_STATE_TAKEN)
	{
		// is moved by carrying entity
	}
	else
	{
		// gravity
		gravity -= actor_gravity*(float)time_delta;
			
		if (state == BOX_STATE_PARACHUTING) gravity = clamp(gravity, -8.f, 8.f);
		else gravity = clamp (gravity, -30, 30);
			
		vec v(0.f, 0.f, gravity*(float)time_delta);
		int result = move_rel_col(&v);
		if (result <= 0)
		{
			gravity = 0.f;
			if (state == BOX_STATE_PARACHUTING)
			{
				state = BOX_STATE_DEFAULT;
			}
		}
	}
}