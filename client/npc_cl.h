#ifndef __NPC_CL_H__
#define __NPC_CL_H__

#include "npc.h"
#include "gameRenderer.h"

class npc_cl : public npc
{
public:
	npc_cl(level *lvl, uint actor_id, int npc_type, vec *, vec*, float health, int target, gameRenderer *arenderer);
	virtual ~npc_cl();
	
	virtual void frame(double time_delta);

protected:
	gameRenderer *renderer;

	RenderObject *ro;
};


#endif
