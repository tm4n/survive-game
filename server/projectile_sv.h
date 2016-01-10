#ifndef __PROJECTILE_SV_H__
#define __PROJECTILE_SV_H__

#include "projectile.h"
#include "player_sv.h"

class projectile_sv : public projectile
{
public:
	projectile_sv(level* lvl, const vec &startpos, const vec &dir, double speed, double damage, int parent_id);
	virtual ~projectile_sv();

	virtual void frame(double time_delta);
	virtual void hit_callback(int actor_hit);

protected:
};


#endif
