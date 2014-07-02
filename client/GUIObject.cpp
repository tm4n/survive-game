#include "GUIObject.h"

GUIObject::GUIObject(Types t, Texture *tex, int layer, Alignment align, float x, float y, GUICallback *ptr)
{
	this->type = t;

	this->layer = layer;
	this->alignment = align;
	this->x = x;
	this->y = y;
	textures.push_back(tex);
	this->callback = ptr;

	// defaults
	scale_x = 1.0f;
	scale_y = 1.0f;
	visible = true;
	centered = false;
	alpha = 1.0f;
	current_tex = 0;

	size_x = tex->size_x;
	size_y = tex->size_y;
}

GUIObject::GUIObject(Types t, std::vector<Texture*> atexs, int layer, Alignment align, float x, float y, GUICallback *ptr)
{
	this->type = t;

	this->layer = layer;
	this->alignment = align;
	this->x = x;
	this->y = y;
	textures.assign(atexs.begin(), atexs.end());
	this->callback = ptr;

	// defaults
	scale_x = 1.0f;
	scale_y = 1.0f;
	visible = true;
	centered = false;
	alpha = 1.0f;
	current_tex = 0;

	size_x = textures[0]->size_x;
	size_y = textures[0]->size_y;
}

GUIObject::~GUIObject()
{
	
}
