#include "projectile_sv.h"
#include "net_sv.h"

projectile_sv::projectile_sv(level* lvl, const vec &startpos, const vec &dir, double speed, double damage, int parent_id) :
projectile(lvl, startpos, dir, speed, damage, parent_id)
{
}

projectile_sv::~projectile_sv()
{
}

void projectile_sv::frame(double time_delta)
{
	if (status == 0 || distance_traveled > PROJECTILE_MAX_TRAVEL)
	{
		delete this;
		return;
	}

	movement(time_delta);
}

void projectile_sv::hit_callback(int actor_hit)
{
	actor *ac = lvl->actorlist.at(actor_hit);
	if (ac != NULL)
	{
		if (ac->health > 0)
		{
			if (ac->type == ACTOR_TYPE_BOX || ac->type == ACTOR_TYPE_PLAYER)
			{
				ac->health -= (float)damage;
				if (ac->health < 0) ac->health = 0;
				net_server->broadcast_update_health(ac->id, ac->health);
			}
			status = 0;
		}
	}
}
