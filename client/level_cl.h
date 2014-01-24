#ifndef __LEVEL_CL_H__
#define __LEVEL_CL_H__

#include "level.h"
#include "gameRenderer.h"
#include "box_cl.h"
#include "player_cl.h"

class level_cl : public level
{
public:
	level_cl(const char *, gameRenderer *arenderer);
	virtual ~level_cl();
	
	player_cl *get_player(uint actor_id);
	box_cl *get_box(uint actor_id);

protected:
	gameRenderer *renderer;

	RenderObject *roTerrain;

};


#endif
