#ifndef __PROJECTILE_CL_H__
#define __PROJECTILE_CL_H__

#include "projectile.h"
#include "Mesh.h"
#include "gameRenderer.h"

class projectile_cl : public projectile
{
public:
	projectile_cl(level* lvl, const vec &startpos, const vec &dir, double speed, double damage, gameRenderer *renderer, Mesh *mesh, int parent_id);
	virtual ~projectile_cl();

	virtual void frame(double time_delta);
	virtual void hit_callback(int actor_hit);

	RenderObject *ro;

	gameRenderer *renderer;
	Mesh *mesh;

protected:
};


#endif
