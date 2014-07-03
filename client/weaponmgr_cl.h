#ifndef __WEAPONMGR_CL_H__
#define __WEAPONMGR_CL_H__

#include "weaponmgr.h"
#include "level.h"
#include "gameRenderer.h"

class weaponmgr_cl : public weaponmgr
{
public:
	weaponmgr_cl(level *lvl, int *curr_weapon, gameRenderer *renderer, int player_id);

	void input_shoot(vec &cam_pos, vec &cam_angle);
	int input_switch(int num);
	void input_scroll_up();
	void input_scroll_down();

protected:
	gameRenderer *renderer;
	int player_id;

};

#endif