#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "vec.h"
#include "level.h"

#define PROJECTILE_MAX_TRAVEL 5000.f

class projectile
{
public:
	projectile(level* lvl, const vec &startpos, const vec &dir, double speed, double damage, int parent_id);
	virtual ~projectile();

	virtual void movement(double time_delta);
	virtual void hit_callback(int actor_hit) = 0;
	virtual void frame(double time_delta) = 0;

protected:
	int id, parent_id, status;
	double speed, damage;
	double distance_traveled;

	vec position, angle;

	level *lvl;
};


#endif // PROJECTILE_H