#include "box_cl.h"

box_cl::box_cl(level *lvl, uint actor_id, char box_type, vec *pos, float health, gameRenderer *arenderer)
	: box(lvl, actor_id, box_type, pos, health)
{
	renderer = arenderer;

	ro = new RenderObject();	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
    
	getMesh()->addRenderObject(ro);

	if (box_type == BOX_TYPE_WOOD || box_type == BOX_TYPE_METAL)
	{
		ro_dmg = new RenderObject();	
		ro_dmg->translation[0] = pos->x;
		ro_dmg->translation[1] = pos->y;
		ro_dmg->translation[2] = pos->z;

		getDmgMesh()->addRenderObject(ro_dmg);
	}
	else ro_dmg = NULL;
}


box_cl::~box_cl()
{
	getMesh()->removeRenderObject(ro);
	delete ro;

	if (box_type == BOX_TYPE_WOOD || box_type == BOX_TYPE_METAL)
	{
		getDmgMesh()->removeRenderObject(ro_dmg);
		delete ro_dmg;
	}
}


Mesh *box_cl::getMesh()
{
	switch(box_type)
	{
	case BOX_TYPE_WOOD:
		return renderer->resources.getMesh(ResourceLoader::meshType::Crate);
		break;
	case BOX_TYPE_METAL:
		return renderer->resources.getMesh(ResourceLoader::meshType::Metalcrate);
		break;
	case BOX_TYPE_TURRET:
		return renderer->resources.getMesh(ResourceLoader::meshType::Turred);
		break;
	case BOX_TYPE_GENERATOR:
		return renderer->resources.getMesh(ResourceLoader::meshType::Generator);
		break;
	}
}

Mesh *box_cl::getDmgMesh()
{
	switch(box_type)
	{
	case BOX_TYPE_WOOD:
		return renderer->resources.getMesh(ResourceLoader::meshType::Crate_damaged);
		break;
	case BOX_TYPE_METAL:
		return renderer->resources.getMesh(ResourceLoader::meshType::Metalcrate_damaged);
		break;
	}
}

void box_cl::frame(double time_delta)
{
	movement(time_delta);
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z; 

	if (box_type == BOX_TYPE_WOOD) ro_dmg->alpha = (100.f - health) / 100.f;
	else
	{
		if (box_type == BOX_TYPE_METAL) ro_dmg->alpha = (100.f - (health / 3.f)) / 100.f;
		else
		{
			if (box_type == BOX_TYPE_GENERATOR)
			{
			  	/*if (old_health != health) 
			  	{
			  		if (!gui_message_visible()) show_message("The generator is under attack! Protect it!");
			  		old_health = my.health;
			  	}*/
			  	ro->coloring[0] = (500.f - health) / 700.f;
			}
			else
			{
				if (box_type == BOX_TYPE_TURRET) ro->coloring[0] = (50.f - health) / 110.f;
		  	}
		}
	}
	 
	// place at the same position as box_cl
	if (ro_dmg != NULL)
	{
		ro_dmg->translation[0] = position.x; ro_dmg->translation[1] = position.y; ro_dmg->translation[2] = position.z; 
		ro_dmg->rotation[0] = angle.x; ro_dmg->rotation[1] = angle.y; ro_dmg->rotation[2] = angle.z; 
	}
}
