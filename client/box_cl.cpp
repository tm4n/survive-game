#include "box_cl.h"
#include "helper.h"

box_cl::box_cl(level *lvl, uint actor_id, char box_type, vec *pos, float health, int target, gameRenderer *arenderer, gui_hud *ahud, effectmgr *aeffmgr)
	: box(lvl, actor_id, box_type, pos, health, target)
{
	this->renderer = arenderer;
	this->hud = ahud;
	this->effmgr = aeffmgr;
	this->old_health = health;
	this->old_target = target;

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
	if (box_type == BOX_TYPE_TURRET)
	{
		ro_wp = new RenderObject(&position, &angle);
		getWpMesh()->addRenderObject(ro_wp);

		ro_wpmf = new RenderObject(&position, &angle);
		renderer->resources.getMesh(ResourceLoader::meshType::Muzzleflash)->addRenderObject(ro_wpmf);
		// TODO: scale?
		ro_wpmf->visible = false;
	}
	else {ro_wp = NULL; ro_wpmf = NULL;}

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

	if (ro_wp != NULL)
	{
		getWpMesh()->removeRenderObject(ro_wp);
		delete ro_wp;
	}

	if (ro_wpmf != NULL)
	{
		renderer->resources.getMesh(ResourceLoader::meshType::Muzzleflash)->removeRenderObject(ro_wpmf);
		delete ro_wpmf;
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

Mesh *box_cl::getWpMesh()
{
	return renderer->resources.getMesh(ResourceLoader::meshType::Turred_MG);
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

	if (ro_wp != NULL)
	{
		ro_wp->translation[0] = position.x; ro_wp->translation[1] = position.y; ro_wp->translation[2] = position.z+22;
	}

	if (ro_wpmf != NULL)
	{
		ro_wpmf->translation[0] = ro_wp->translation[0]; ro_wpmf->translation[1] = ro_wp->translation[1]; ro_wpmf->translation[2] = ro_wp->translation[2];
		// Move forward
		ro_wpmf->translation[0] += cos(toRadians(ro_wp->rotation[0]))*cos(toRadians(-ro_wp->rotation[1]))*22.f;
		ro_wpmf->translation[1] += sin(toRadians(ro_wp->rotation[0]))*cos(toRadians(-ro_wp->rotation[1]))*22.f;
		ro_wpmf->translation[2] += sin(toRadians(-ro_wp->rotation[1]))*22.f;
		
		ro_wpmf->rotation[0] = ro_wp->rotation[0]; ro_wpmf->rotation[1] = ro_wp->rotation[1]; ro_wpmf->rotation[2] = ro_wp->rotation[2];

		if (ro_wpmf->alpha > 0.f) ro_wpmf->alpha -= (float)time_delta;
		else ro_wpmf->visible = false;
	}


	///////////////////////////////////////////////
	// turret
	if (box_type == BOX_TYPE_TURRET)
	{
		// continous attack
		attack_count += (double)attack_speed*time_delta;

		// attack target
		if (target >= 0)
		{
			actor *t = lvl->actorlist.at(target);
			if (t != NULL)
			{
				vec v(t->position.x - ro_wp->translation[0], t->position.y - ro_wp->translation[1], t->position.z - ro_wp->translation[2]);
				ro_wp->rotation[0] = vec::angle(90.0f - vec::angle(atan2(v.x, v.y))*(float)(180.0 / M_PI));
				ro_wp->rotation[1] = -vec::angle(asin(v.z / v.length())*(float)(180.0 / M_PI));


				if (target != old_target)
				{
					old_target = target;
					if (target >= 0) renderer->resources.getSnd(ResourceLoader::sndType::Turret)->play3D(1, ro_wp, 50);
				}

				if (attack_count > 100.)
				{
					// show muzzle flash, make sound effect
					renderer->resources.getSnd(ResourceLoader::sndType::Colt_shot)->play3D(1, ro_wp, 50);
					
					ro_wpmf->rotation[2] = random_range(360.f);
					ro_wpmf->alpha = 0.8f;
					ro_wpmf->visible = true;

					// get random target vector
					vec shoot_origin(ro_wpmf->translation[0], ro_wpmf->translation[1], ro_wpmf->translation[2]);
					// move forward a bit more
					shoot_origin.x += cos(toRadians(ro_wp->rotation[0]))*cos(toRadians(-ro_wp->rotation[1]))*40.f;
					shoot_origin.y += sin(toRadians(ro_wp->rotation[0]))*cos(toRadians(-ro_wp->rotation[1]))*40.f;
					shoot_origin.y += sin(toRadians(-ro_wp->rotation[1]))*40.f;

					vec shoot_target(t->position.x + random_range(10.f) - 5.f, t->position.y + random_range(10.f) - 5.f, t->position.z + random_range(10.f) - 5.f);
					vec shoot_dir(ro_wp->rotation[0], ro_wp->rotation[1], ro_wp->rotation[2]);
					effmgr->eff_bullettrail(&shoot_origin, &shoot_target);

					effmgr->eff_blood(&shoot_target, &shoot_origin, &shoot_dir);

					attack_count = 0.;
				}
			}
		}
		else
		{
			ro_wp->rotation[1] = 40.;
		}
	}

}
