#include "npc_cl.h"

npc_cl::npc_cl(level *lvl, uint actor_id, int npc_type, vec *pos, vec*ang, float health, int target, gameRenderer *arenderer)
	: npc(lvl, actor_id, npc_type, pos, ang, health, target)
{
	renderer = arenderer;

	ro = new RenderObject();
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
	ro->rotation[0] = ang->x;
	ro->rotation[1] = ang->y;
	ro->rotation[2] = ang->z;
    
	renderer->resources.getMesh(b_npcs::instance()->at(npc_type)->res_mesh)->addRenderObject(ro);

    
}


npc_cl::~npc_cl()
{
	renderer->resources.getMesh(b_npcs::instance()->at(npc_type)->res_mesh)->removeRenderObject(ro);

	delete ro;
}




void npc_cl::frame(double time_delta)
{
	//movement(time_delta);
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z; 
}
 
