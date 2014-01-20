#ifndef __LEVEL_SV_H__
#define __LEVEL_SV_H__

#include "level.h"
#include "game.h"
#include <list>
#include "box_sv.h"
#include "player_sv.h"
class player_sv;



class level_sv : public level
{
    public:
        level_sv(const char *);
        ~level_sv();
        
        void spawn_starters();

        player_sv *get_player(uint actor_id);
        box_sv *get_box(uint actor_id);
        //npc_sv *get_npc(uint actor_id);

    protected:
    private:
};

#endif // __B_LEVEL_SV_H__
