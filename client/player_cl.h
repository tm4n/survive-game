#ifndef __PLAYER_CL_H__
#define __PLAYER_CL_H__

#include "player.h"
#include "gameRenderer.h"
#include "weaponmgr_cl.h"
#include "effectmgr.h"

class player_cl : public player
{
public:
	bool local_player;

	bool input_shoot;

	RenderObject *ro;

	weaponmgr_cl *wpmgr;

	player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int weapon, int input, int object_taken, gameRenderer *arenderer, effectmgr *effmgr);
	virtual ~player_cl();

	virtual void frame(double time_delta);

	void animate(double time_delta);
	
	void order_take_object();

protected:
	gameRenderer *renderer;

	float send_angle_timer, send_pos_timer;

	double step_count;
	bool step_first;

	int old_anim;
	float anim_prog;

	void snd_ent_step(bool dir);
};


#endif
