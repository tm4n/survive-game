#ifndef __BOX_CL_H__
#define __BOX_CL_H__

#include "box.h"
#include "gameRenderer.h"

class box_cl : public box
{
public:
	box_cl(level *lvl, uint actor_id, char box_type, vec *, float health, gameRenderer *arenderer);
	virtual ~box_cl();
	
	virtual void frame(double time_delta);

	Mesh *getMesh();
	Mesh *getDmgMesh();

protected:
	gameRenderer *renderer;


	RenderObject *ro, *ro_dmg;
};


#endif
