#ifndef __NPC_SV_H__
#define __NPC_SV_H__

#include "npc.h"
#include "game.h"
#include "level_sv.h"
#include "actor_updater.h"
#include "npc_ai.h"

class actor_updater;
class path;
class npc_ai;
class level_sv;

class npc_sv : public npc
{
    public:
        vec spawn_position;
        actor_updater *updater;
        path *pathfinder;
        npc_ai *ai;

        npc_sv(level_sv *, uint, vec *, vec *);
        virtual ~npc_sv();

        void reset_position();
        virtual void frame(double time_delta);
        virtual void set_state(uint new_state);
        void loot_remove();
        void loot_disassociate();

        virtual bool do_damage(uint target_actor, float damage);

    protected:
    private:
};

#endif // __NPC_SV_H__
