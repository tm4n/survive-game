#ifndef __PLAYER_SV_H__
#define __PLAYER_SV_H__

#include "gameServer.h"
#include "player.h"
#include <enet/enet.h>
#include "level_sv.h"
#include "enet/enet.h"
#include "weaponmgr_sv.h"

class player_sv : public player
{
    public:
        ENetPeer *owner;

		weaponmgr_sv *wpmgr;


        player_sv(level *, vec *pos, vec *ang, float health,
               const char *name, ENetPeer *owner);

        virtual ~player_sv();

        virtual void frame (double time_delta);
        
		int ang_count;

    protected:

		float send_pos_timer;
		vec last_position;
		float old_health;

    private:
};

#endif // PLAYER_SV_H
