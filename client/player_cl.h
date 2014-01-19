#ifndef __PLAYER_CL_H__
#define __PLAYER_CL_H__

#include "player.h"
#include "gameRenderer.h"

class player_cl : public player
{
public:
	
	player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int state, int input, gameRenderer *arenderer);
	virtual ~player_cl();

protected:
	gameRenderer *renderer;

	RenderObject *ro;
};


#endif