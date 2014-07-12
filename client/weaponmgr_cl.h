#ifndef __WEAPONMGR_CL_H__
#define __WEAPONMGR_CL_H__

#include "weaponmgr.h"
#include "level.h"
#include "gameRenderer.h"

class weaponmgr_cl : public weaponmgr
{
public:
	weaponmgr_cl(level *lvl, int *curr_weapon, bool *local_player, gameRenderer *renderer, int player_id);
	~weaponmgr_cl();

	void input_shoot(vec &cam_pos, vec &cam_angle);
	int input_switch(int num);
	void input_scroll_up();
	void input_scroll_down();

	void frame(double time_frame);

	void set_mag_ammo(int weapon_id, short magazin, short ammo);
	void switch_cl(int new_weapon_id);

protected:
	bool *local_player;
	gameRenderer *renderer;
	int player_id;

	int anim_state;
	float anim_count;

	RenderObject *ro;

	void set_anim_state(int new_state);
	Mesh *getMesh(int weapon_id);

};

#endif