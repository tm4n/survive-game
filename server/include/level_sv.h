#ifndef __LEVEL_SV_H__
#define __LEVEL_SV_H__

#include "level.h"
#include "game.h"
#include "npc_sv.h"
#include "player_sv.h"
#include <list>

class npc_sv;
class player_sv;


class level_sv : public level
{
    public:
        level_sv(const char *);
        ~level_sv();

        player_sv *get_player(uint actor_id);
        npc_sv *get_npc(uint actor_id);

    protected:
    private:
};

#endif // __B_LEVEL_SV_H__
