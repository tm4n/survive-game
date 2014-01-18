#ifndef __LEVEL_CL_H__
#define __LEVEL_CL_H__

#include "level.h"
#include "gameRenderer.h"

class level_cl : public level
{
public:
	level_cl(const char *, gameRenderer *arenderer);
	virtual ~level_cl();

protected:
	gameRenderer *renderer;

	RenderObject *roTerrain;

};


#endif