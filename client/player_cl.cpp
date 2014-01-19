#include "player_cl.h"

player_cl::player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int state, int input, gameRenderer *arenderer)
	: player(lvl, actor_id, pos, ang, health, name, state, input)
{
	renderer = arenderer;

	ro = new RenderObject();
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
        	
    renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(ro);
}


player_cl::~player_cl()
{
	renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->removeRenderObject(ro);

	delete ro;
}

