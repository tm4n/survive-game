#ifndef __WEAPONMGR_CL_H__
#define __WEAPONMGR_CL_H__

#include "weaponmgr.h"
#include "level.h"
#include "gameRenderer.h"
#include "MeshGUI.h"
#include "effectmgr.h"

class weaponmgr_cl : public weaponmgr
{
public:
	weaponmgr_cl(level *lvl, int *curr_weapon, bool *local_player, ushort *plstate, gameRenderer *renderer, effectmgr *effmgr, int player_id);
	~weaponmgr_cl();

	void input_shoot(vec &cam_pos, vec &cam_angle);
	int input_switch(int num);
	void input_scroll_up();
	void input_scroll_down();
	void input_reload();

	void frame(double time_frame, vec &pl_pos, vec &pl_angle, int frame);

	void set_mag_ammo(int weapon_id, short magazin, short ammo);
	void switch_cl(int new_weapon_id);
	void update_curr_weapon(int new_curr_weapon);
	void shoot(vec &shoot_origin, vec &shoot_dir, int random_seed);
	void reload();
	void cancel_reload();

	void hide_wp();
	void show_wp();

	static void snd_hit(ResourceLoader *res, actor *ac, float vol_modifier = 1.0f);

protected:
	bool *local_player;
	ushort *plstate;
	gameRenderer *renderer;
	effectmgr *effmgr;
	int player_id;

	int anim_state;
	float anim_count;
	vec recoil;

	bool hidden;

	RenderObject *ro_h, *ro_w, *ro_mf;
	MeshGUI *curr_handmesh;
	Mesh *curr_wpmesh;

	void set_anim_state(int new_state);
	MeshGUI *get_handmesh(int weapon_id);
	Mesh *get_wpmesh(int weapon_id);
	void show_muzzleflash(int weapon_id);

};

#endif
