#ifndef __PLAYER_CL_H__
#define __PLAYER_CL_H__

#include "player.h"
#include "gameRenderer.h"

class player_cl : public player
{
public:

	RenderObject *ro;

	player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int weapon, int input, int object_taken, gameRenderer *arenderer);
	virtual ~player_cl();

	virtual void frame(double time_delta);


protected:
	gameRenderer *renderer;
};


#endif
