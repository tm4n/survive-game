#include "projectile.h"
#include "helper.h"

projectile::projectile(level* lvl, const vec &startpos, const vec &dir, double speed, double damage, int parent_id) :
lvl(lvl), position(startpos), angle(dir), speed(speed), damage(damage), parent_id(parent_id)
{
	distance_traveled = 0.;
	status = 1;

	id = lvl->projectilelist.add(this);
}

projectile::~projectile()
{
	// remove me from projectilelist
	lvl->projectilelist.remove(id);
}

void projectile::movement(double time_delta)
{
	// check position
	if (parent_id >= 0)
	{
		if (lvl->actorlist.at(parent_id) == NULL) parent_id = -1;
	}

	// calculate position to move to
	vec pos_to(position);
	vec old_pos(position);
	move_dir(pos_to, angle, (float)(speed*time_delta), 0.f, 0.f);
	position.set(&pos_to);

	vec pos_hit;
	int actor_hit;
	if (lvl->trace(position, pos_to, &pos_hit, &actor_hit, parent_id))
	{
		position.set(&pos_hit);

		hit_callback(actor_hit);
	}
	else position.set(&pos_to);

	distance_traveled += old_pos.dist(&position);
}