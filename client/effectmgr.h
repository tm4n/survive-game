#ifndef __EFFECTMGR_H__
#define __EFFECTMGR_H__

#include "gameRenderer.h"
#include "RenderObject.h"


typedef struct {
	RenderObject *ro;
	float dist_traveled;
	float to_travel;
} bullet_entry;

class effectmgr
{
public:
	effectmgr(gameRenderer *renderer);
	virtual ~effectmgr();

	void eff_bullettrail(const vec *start, const vec *end);
	void eff_pl_flash(int color);

	void frame(double frame_delta);

protected:
	gameRenderer *renderer;

	std::list<bullet_entry*> bullets;

	int flashred_id, flashgreen_id;
};

#endif