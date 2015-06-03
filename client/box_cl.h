#ifndef __BOX_CL_H__
#define __BOX_CL_H__

#include "box.h"
#include "gameRenderer.h"
#include "gui_hud.h"
#include "effectmgr.h"

class box_cl : public box
{
public:
	box_cl(level *lvl, uint actor_id, char box_type, vec *, float health, int target, gameRenderer *arenderer, gui_hud *hud, effectmgr *effmgr);
	virtual ~box_cl();
	
	virtual void frame(double time_delta);

	Mesh *getMesh();
	Mesh *getDmgMesh();
	Mesh *getWpMesh();

	RenderObject *ro, *ro_dmg, *ro_parachute, *ro_wp, *ro_wpmf;

protected:
	gameRenderer *renderer;
	gui_hud *hud;
	effectmgr *effmgr;
	float old_health;
	int old_target;
};


#endif
