#include "projectile_cl.h"

projectile_cl::projectile_cl(level* lvl, const vec &startpos, const vec &dir, double speed, double damage, gameRenderer *renderer, Mesh *mesh, int parent_id) :
projectile(lvl, startpos, dir, speed, damage, parent_id), mesh(mesh), renderer(renderer)
{
	ro = new RenderObject();
	ro->translation[0] = startpos.x;
	ro->translation[1] = startpos.y;
	ro->translation[2] = startpos.z;

	mesh->addRenderObject(ro);
}

projectile_cl::~projectile_cl()
{
	mesh->removeRenderObject(ro);
	delete ro;
}

void projectile_cl::frame(double time_delta)
{
	if (distance_traveled > PROJECTILE_MAX_TRAVEL || status == -1)
	{
		delete this;
		return;
	}

	if (status == 1) movement(time_delta);

	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z;
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z;
}

void projectile_cl::hit_callback(int actor_hit)
{
	actor *ac = lvl->actorlist.at(actor_hit);
	if (ac != NULL)
	{
		if (ac->health > 0)
		{
			status = -1;
		}
	}
}
