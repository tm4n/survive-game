#include "box_cl.h"

box_cl::box_cl(level *lvl, uint actor_id, char box_type, vec *pos, float health, gameRenderer *arenderer)
	: box(lvl, actor_id, box_type, pos, health)
{
	renderer = arenderer;

	ro = new RenderObject();
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
        	
    renderer->resources.getMesh(ResourceLoader::meshType::Crate)->addRenderObject(ro);
}


box_cl::~box_cl()
{
	renderer->resources.getMesh(ResourceLoader::meshType::Crate)->removeRenderObject(ro);

	delete ro;
}

