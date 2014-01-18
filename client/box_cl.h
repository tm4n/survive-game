#ifndef __BOX_CL_H__
#define __BOX_CL_H__

#include "box.h"
#include "gameRenderer.h"

class box_cl : public box
{
public:
	box_cl(level *lvl, uint actor_id, char box_type, vec *, uint health, gameRenderer *arenderer);
	virtual ~box_cl();

protected:
	gameRenderer *renderer;

	RenderObject *ro;
};


#endif