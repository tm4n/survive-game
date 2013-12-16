#include "GUIObject.h"

GUIObject::GUIObject(Texture *tex, int layer, float x, float y)
{
	this->layer = layer;
	this->x = x;
	this->y = y;
	this->tex = tex;

	// defaults
	scale_x = 1.0f;
	scale_y = 1.0f;
	visible = true;
	centered = false;
	alpha = 1.0f;

	size_x = 1.0f;
	size_y = 1.0f;
}