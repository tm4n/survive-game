#include "effectmgr.h"
#include "helper.h"
#include <algorithm>

effectmgr::effectmgr(gameRenderer *renderer)
	: renderer(renderer)
{
	// creates panels
	flashred_id = renderer->gui->addPanel(renderer->resources.getTex(ResourceLoader::texType::FlashRed), 1, GUIObject::Alignment::scaled, 0.0f, 0.0f);
	flashgreen_id = renderer->gui->addPanel(renderer->resources.getTex(ResourceLoader::texType::FlashGreen), 1, GUIObject::Alignment::scaled, 0.0f, 0.0f);
	renderer->gui->setVisible(flashred_id, false);
	renderer->gui->setAlpha(flashred_id, 0.0f);
	renderer->gui->setScaleX(flashred_id, 1920.f/640.f);
	renderer->gui->setScaleY(flashred_id, 1080.f/480.f);

	renderer->gui->setVisible(flashgreen_id, false);
	renderer->gui->setAlpha(flashgreen_id, 0.0f);
	renderer->gui->setScaleX(flashgreen_id, 1920.f/640.f);
	renderer->gui->setScaleY(flashgreen_id, 1080.f/480.f);
}

effectmgr::~effectmgr()
{
	// delete bullets
	for(bullet_entry* i : bullets)
	{
		renderer->resources.getMesh(ResourceLoader::meshType::Bullet)->removeRenderObject(i->ro);
		delete i->ro;
		delete i;
	}

	// delete panels
	renderer->gui->removeObject(flashred_id);
	renderer->gui->removeObject(flashgreen_id);
}

void effectmgr::eff_bullettrail(const vec *start, const vec *end)
{
	if (start->dist(end) < 200.f) return;

	bullet_entry *b = new bullet_entry;

	RenderObject *ro = new RenderObject();
	ro->translation[0] = start->x;
	ro->translation[1] = start->y;
	ro->translation[2] = start->z;

	vec v (end->x - start->x, end->y - start->y, end->z - start->z);
	ro->rotation[0] = vec::angle(90.0f - vec::angle(atan2(v.x, v.y))*(float)(180.0/M_PI));
	ro->rotation[1] = -vec::angle(asin(v.z / v.length())*(float)(180.0/M_PI));
	ro->rotation[2] = 0.f;

	renderer->resources.getMesh(ResourceLoader::meshType::Bullet)->addRenderObject(ro);

	b->to_travel = start->dist(end);
	b->dist_traveled = 0.f;
	b->ro = ro;

	bullets.push_back(b);
}

// flashing
void effectmgr::eff_pl_flash(int color)
{
	// TODO: implement. is already called
	if (color  == 1) // red
	{
		renderer->gui->setVisible(flashred_id, true);
		renderer->gui->setAlpha(flashred_id, 0.5f);
	}
	else
	{
		renderer->gui->setVisible(flashgreen_id, true);
		renderer->gui->setAlpha(flashgreen_id, 0.5f);
	}
}

// wall hit particle effect
void wallhit_cb(particle_data *pdata, float time_delta)
{
	pdata->alpha -= 0.08f*time_delta;
	if (pdata->alpha <= 0.f) pdata->lifespan = 0.f;
}

void wallhit_init(particle_data *pdata)
{
	pdata->vel.x *= random_range(5.f) - 1.f;
	pdata->vel.y *= random_range(5.f) - 1.f;
	pdata->vel.z *= random_range(5.f) - 1.f+5.f;

	pdata->vel.x += random_range(6.f)-3.f;
	pdata->vel.y += random_range(6.f)-3.f;
	pdata->vel.z += random_range(6.f)-3.f;

	pdata->gravity = 3.f;

	pdata->lifespan = 9999.f;

	//p.red = 255; p.green = 255; p.blue = 100;

	pdata->size = 1.5f;
	pdata->alpha = 0.75f + random_range(0.25f);

	//set(p, TRANSLUCENT | STREAK | MOVE);
   //p.event= eff_hit_fade; // change to a shorter, faster function
}

void effectmgr::eff_wallhit(const vec *position, const vec *normal)
{
	renderer->partmgr->emit(10, ResourceLoader::texType::pSpark, &wallhit_cb, &wallhit_init, position, normal);
}


// blood particle effect
void blood_cb(particle_data *pdata, float time_delta)
{
	pdata->alpha -= 0.03f*time_delta;
	if (pdata->alpha <= 0.f) pdata->lifespan = 0.f;
}

void blood_init(particle_data *pdata)
{
	pdata->vel.x *= random_range(5.f) - 6.f;
	pdata->vel.y *= random_range(5.f) - 6.f;
	pdata->vel.z *= random_range(1.f) - 2.f;

	pdata->gravity = 0.6f;

	pdata->lifespan = 9999.f;

	//p.green = 0; p.blue = 0; p.red = 140;

	pdata->size = 1.f;
	pdata->alpha = 0.75f + random_range(0.25f);
	//set (p, TRANSLUCENT | MOVE | STREAK);
}

void blood2_cb(particle_data *pdata, float time_delta)
{
	pdata->alpha -= 0.05f*time_delta;
	if (pdata->alpha <= 0.f) pdata->lifespan = 0.f;
}

void blood2_init(particle_data *pdata)
{
	pdata->vel.x = random_range(2.f) - 1.f;
	pdata->vel.y = random_range(2.f) - 1.f;
	pdata->vel.z = random_range(2.f) - 1.f;

	pdata->gravity = 0.f;

	pdata->lifespan = 9999.f;

	//p.green = 0; p.blue = 0; p.red = 141;

	pdata->size = 16.f;
	pdata->alpha = 0.5f;
	//set (p, TRANSLUCENT | MOVE);
}

void effectmgr::eff_blood(const vec *target_position, const vec *mypos, const vec *youpos)
{
	vec vnull(0, 0, 0);
	renderer->partmgr->emit(15, ResourceLoader::texType::pBlood2, &blood2_cb, &blood2_init, target_position, &vnull);


	vec v(target_position->x - mypos->x, target_position->y - mypos->y, target_position->z - mypos->z);
	float dir = vec::angle(90.0f - vec::angle(atan2(v.x, v.y))*(float)(180.0 / M_PI));
	dir += random_range(30) - 15;

	vec vel;
	vel.x = cos(toRadians(dir))*-1.f;
	vel.y = sin(toRadians(dir))*-1.f;

	vnull.x = 1.0f;
	renderer->partmgr->emit(4, ResourceLoader::texType::pBlood1, &blood_cb, &blood_init, target_position, &vel);
}



void effectmgr::frame(double frame_delta)
{

	// bullets
	std::list<bullet_entry*>::iterator i = bullets.begin();
	while (i != bullets.end())
	{
		if ((*i)->dist_traveled >= (*i)->to_travel)
		{
			renderer->resources.getMesh(ResourceLoader::meshType::Bullet)->removeRenderObject((*i)->ro);
			delete (*i)->ro;
			delete *i;

			i = bullets.erase(i);
		}
		else
		{
			glm::vec3 v;
			v[0] = (*i)->ro->rotation[0]; v[1] = -(*i)->ro->rotation[1]; v[2] = (*i)->ro->rotation[2];
			move_dir((*i)->ro->translation, v, (float)frame_delta*400.f, 0.f, 0.f);

			(*i)->dist_traveled += (float)frame_delta*400.f;

			++i;
		}
	}

	// flash effects
	float alpha = renderer->gui->getAlpha(flashred_id);
	if (alpha > 0.0f)
	{
		renderer->gui->setAlpha(flashred_id, alpha - 0.1f*((float)frame_delta));
		if (alpha - 0.1f*((float)frame_delta) <= 0.0f) 
		{
			renderer->gui->setAlpha(flashred_id, 0.0f);
			renderer->gui->setVisible(flashred_id, false);
		}
	}
	alpha = renderer->gui->getAlpha(flashgreen_id);
	if (alpha > 0.0f)
	{
		renderer->gui->setAlpha(flashgreen_id, alpha - 0.1f*((float)frame_delta));
		if (alpha - 0.1f*((float)frame_delta) <= 0.0f) 
		{
			renderer->gui->setAlpha(flashgreen_id, 0.0f);
			renderer->gui->setVisible(flashgreen_id, false);
		}
	}
}