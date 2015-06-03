#include "box_cl.h"

box_cl::box_cl(level *lvl, uint actor_id, char box_type, vec *pos, float health, int target, gameRenderer *arenderer, gui_hud *ahud)
	: box(lvl, actor_id, box_type, pos, health, target)
{
	this->renderer = arenderer;
	this->hud = ahud;
	this->old_health = health;

	ro = new RenderObject();	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
    
	getMesh()->addRenderObject(ro);

	if (box_type == BOX_TYPE_WOOD || box_type == BOX_TYPE_METAL)
	{
		ro_dmg = new RenderObject(&position, &angle);	
		getDmgMesh()->addRenderObject(ro_dmg);
	}
	else ro_dmg = NULL;

	if (box_type == BOX_TYPE_GENERATOR)
	{
		renderer->resources.getSnd(ResourceLoader::sndType::Generator)->play3D(0, ro, 30);
	}

	if (state == BOX_STATE_PARACHUTING)
	{
		ro_parachute = new RenderObject(&position, &angle);
		renderer->resources.getMesh(ResourceLoader::meshType::Parachute)->addRenderObject(ro_parachute);
	}
	else ro_parachute = NULL;

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

	if (ro_parachute != NULL)
	{
		renderer->resources.getMesh(ResourceLoader::meshType::Parachute)->removeRenderObject(ro_parachute);
		delete ro_parachute;
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
	
	return NULL;
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
	
	return NULL;
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
			  	if (old_health != health) 
			  	{
			  		if (!hud->message_visible()) hud->show_message("The generator is under attack! Protect it!");
			  		old_health = health;
			  	}
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

	if (ro_parachute != NULL)
	{
		if (state != BOX_STATE_PARACHUTING)
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


	///////////////////////////////////////////////
	// turret
	if (box_type == BOX_TYPE_TURRET)
	{
		// continous attack
		attack_count += (double)attack_speed*time_delta;

		// attack target
		/*if (target >= 0)
		{
			ptr_temp = enet_ent_locpointer(my.target_ent - 1);
			if (ptr_temp != NULL)
			{
				vec_set(vtemp, ptr_temp.x);
				vec_sub(vtemp, ent_gun.x);
				vec_to_angle(ent_gun.pan, vtemp);

				if (my.target_ent != old_target_ent)
				{
					old_target_ent = my.target_ent;
					if (my.target_ent != NULL) snd_turret(1);
				}

				if (my.attack_count > 100)
				{
					// show muzzle flash, make sound effect
					snd_ent_shoot(1);
					eff_muzzleflash(ent_gun);

					// get random target vector
					vec_set(target, ptr_temp.x);
					vec_add(target, vector(random(10) - 5, random(10) - 5, random(10) - 5));
					eff_bullettrail(ent_gun.x, target);

					you = ptr_temp;
					eff_blood();
						you = NULL;

					my.attack_count = 0;
				}
			}
		}
		else
		{
			ent_gun.tilt = -40;
		}*/
	}

}
