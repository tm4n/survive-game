#include "RenderObject.h"

RenderObject::RenderObject()
{
	translation[0] = 0.f;
	translation[1] = 0.f;
	translation[2] = 0.f;
	
	rotation[0] = 0.f;
	rotation[1] = 0.f;
	rotation[2] = 0.f;

	scale[0] = 1.f;
	scale[1] = 1.f;
	scale[2] = 1.f;
	
	animFrame = 0;
	animNextFrame = 0;
	animProgress = 0.0f;
}

RenderObject::RenderObject(float *atranslation, float *arotation)
{
	translation[0] = atranslation[0];
	translation[1] = atranslation[1];
	translation[2] = atranslation[2];
	
	rotation[0] = arotation[0];
	rotation[1] = arotation[1];
	rotation[2] = arotation[2];

	scale[0] = 1.f;
	scale[1] = 1.f;
	scale[2] = 1.f;
	
	animFrame = 0;
	animNextFrame = 0;
	animProgress = 0.0f;
}