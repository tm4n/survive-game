#ifndef __RENDEROBJECT_H__
#define __RENDEROBJECT_H__

#include <glm/glm.hpp>

class RenderObject
{
public:
	glm::vec3 translation;
	
	glm::vec3 rotation;
	
	glm::vec3 scale;
	
	int animFrame;
	int animNextFrame;
	float animProgress;
	float alpha;
	glm::vec3 coloring;

	bool visible;

	RenderObject();
	RenderObject(float *translation, float *rotation);
};


#endif