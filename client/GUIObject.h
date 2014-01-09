#ifndef __GUIOBJECT_H__
#define __GUIOBJECT_H__

#include "Texture.h"

#include <vector>

class GUICallback {

	public:
		virtual void callback(int obj_id) = 0;

};


class GUIObject
{
public:
	enum Types {button, panel, text};

	Types type;

	std::vector<Texture*> textures;

	GUICallback *callback;

	int layer;
	float x,y;
	int size_x, size_y;
	float scale_x, scale_y;
	float alpha;

	int current_tex;

	bool visible;
	bool centered;

	GUIObject(GUIObject::Types, Texture *tex, int layer, float x, float y, GUICallback *);
	GUIObject(GUIObject::Types, std::vector<Texture*>, int layer, float x, float y, GUICallback *);
};


#endif
