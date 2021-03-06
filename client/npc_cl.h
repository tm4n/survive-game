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
	void animate(double time_delta);

	virtual void callback_attack_done(int target, const vec &target_pos);

	Mesh* getMesh() {return renderer->resources.getMesh(b_npcs::instance()->at(npc_type)->res_mesh);}

	RenderObject *ro;

protected:
	int old_anim;
	float anim_prog;

	gameRenderer *renderer;

	void snd_taunt();
};


#endif
