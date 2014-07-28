#include "effectmgr.h"
#include "helper.h"

effectmgr::effectmgr(gameRenderer *renderer)
	: renderer(renderer)
{
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
}