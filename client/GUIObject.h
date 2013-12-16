#ifndef __GUIOBJECT_H__
#define __GUIOBJECT_H__

#include "Texture.h"

class GUIObject
{
public:
	enum Types {button, panel, text};

	Types type;

	Texture *tex;

	int layer;
	float x,y;
	int size_x, size_y;
	float scale_x, scale_y;
	float alpha;

	bool visible;
	bool centered;

	GUIObject(Texture *tex, int layer, float x, float y);
};


#endif