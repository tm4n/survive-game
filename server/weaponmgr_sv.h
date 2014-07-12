#ifndef __WEAPONMGR_SV_H__
#define __WEAPONMGR_SV_H__

#include "weaponmgr.h"
#include "enet/enet.h"


class weaponmgr_sv : public weaponmgr
{
public:
	weaponmgr_sv(level *lvl, int *curr_weapon, ENetPeer *playerpeer, int player_id);

	void give_weapon(int weapon_id);

	void shoot(vec &shoot_origin, vec &shoot_dir);

	void frame(float time_frame);


protected:
	ENetPeer *playerpeer;
	int player_id;
};

#endif