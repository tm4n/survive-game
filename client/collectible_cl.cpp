#include "collectible_cl.h"


collectible_cl::collectible_cl(level *lvl, uint actor_id, char collectible_type, vec *pos, gameRenderer *arenderer)
	: collectible(lvl, actor_id, collectible_type, pos)
{
	renderer = arenderer;
	
	ro = new RenderObject();
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
	getMesh()->addRenderObject(ro);
	
	ro_wp = NULL;
	Mesh *wpm = getWpMesh();
	if (wpm != NULL)
	{
		ro_wp->rotation[0] = 85.f; ro_wp->rotation[1] = 0.f; ro_wp->rotation[2] = 90.f;
		wpm->addRenderObject(ro_wp);
	}
	
	if (state == COLLECTIBLE_STATE_PARACHUTE)
	{
		ro_parachute = new RenderObject(&position, &angle);
		renderer->resources.getMesh(ResourceLoader::meshType::Parachute)->addRenderObject(ro_parachute);
	}
	else ro_parachute = NULL;
}


collectible_cl::~collectible_cl()
{
	getMesh()->removeRenderObject(ro);
	
	Mesh *wpm = getWpMesh();
	if (wpm != NULL)
	{
		wpm->removeRenderObject(ro_wp);
	}
	
	if (ro_parachute != NULL)
	{
		renderer->resources.getMesh(ResourceLoader::meshType::Parachute)->removeRenderObject(ro_parachute);
		delete ro_parachute;
	}
}



Mesh *collectible_cl::getMesh()
{
	return renderer->resources.getMesh(ResourceLoader::meshType::Weaponcrate);
}

Mesh *collectible_cl::getWpMesh()
{
	//return renderer->resources.getMesh(ResourceLoader::meshType::Crate);
	return NULL;
}



// frame callback

void collectible_cl::frame(double time_delta)
{
	movement(time_delta);
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z;
	
	if (ro_wp != NULL)
	{
		ro->translation[0] = position.x - 4.f; ro->translation[1] = position.y - 15.f; ro->translation[2] = position.z+11; 
	}
	
	if (ro_parachute != NULL)
	{
		if (state != COLLECTIBLE_STATE_PARACHUTE)
		{
			// remove parachute
			renderer->resources.getMesh(ResourceLoader::meshType::Parachute)->removeRenderObject(ro_parachute);
			delete ro_parachute;
			ro_parachute = NULL;
		}
		else
		{
			ro_parachute->translation[0] = position.x; ro_parachute->translation[1] = position.y; ro_parachute->translation[2] = position.z; 
			ro_parachute->rotation[0] = angle.x; ro_parachute->rotation[1] = angle.y; ro_parachute->rotation[2] = angle.z; 
		}
	}
}
