#ifndef __PLAYER_SV_H__
#define __PLAYER_SV_H__

#include "gameServer.h"
#include "player.h"
#include <enet/enet.h>
#include "level_sv.h"
#include "enet/enet.h"

class player_sv : public player
{
    public:
        ENetPeer *owner;


        player_sv(level *, vec *pos, vec *ang, float health,
               const char *name, ENetPeer *owner);

        virtual ~player_sv();

        //virtual void frame (double time_delta);
    protected:

    private:
};

#endif // PLAYER_SV_H
