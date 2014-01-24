#include "level_cl.h"

level_cl::level_cl(const char *filename, gameRenderer *arenderer)
	: level(filename)
{
	renderer = arenderer;

	// create terrain
	roTerrain = new RenderObject();
	roTerrain->scale = glm::vec3(2.0f, 2.0f, 2.0f);
    renderer->resources.getMesh(ResourceLoader::meshType::Terrain)->addRenderObject(roTerrain);

	// create trees at their location
	RenderObject *tmp = new RenderObject();
	tmp->translation = glm::vec3(-328.0f, -1400.0f, -312.0f);
	tmp->rotation[0] = 200.0f;
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
	tmp = new RenderObject();
	tmp->translation = glm::vec3(800.0f, 1296.0f, -300.0f);
	tmp->rotation[0] = 36.0f;
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
	tmp = new RenderObject();
	tmp->rotation[0] = 6.0f;
	tmp->translation = glm::vec3(-1440.0f, -784.0f, -300.0f);
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
}

level_cl::~level_cl()
{
}


player_cl *level_cl::get_player(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_PLAYER) return (player_cl*)ac;
    return NULL;
}

box_cl *level_cl::get_box(uint actor_id)
{
    actor *ac = actorlist.at(actor_id);
    if (ac == NULL) return NULL;

    if (ac->type == ACTOR_TYPE_BOX) return (box_cl*)ac;
    return NULL;
}
