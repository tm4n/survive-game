#ifndef __COLLECTIBLE_CL_H__
#define __COLLECTIBLE_CL_H__

#include "collectible.h"
#include "gameRenderer.h"


class collectible_cl : public collectible
{
	public:
		collectible_cl(level *lvl, uint actor_id, char collectible_type, vec *pos, gameRenderer *renderer);
		virtual ~collectible_cl();
		
		Mesh *getMesh();
		Mesh *getWpMesh();
		
		virtual void frame(double time_delta);
		
		RenderObject *ro, *ro_wp, *ro_parachute;
	protected:
		gameRenderer *renderer;
	
	private:
};

#endif
