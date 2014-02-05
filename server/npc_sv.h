#ifndef __NPC_SV_H__
#define __NPC_SV_H__

#include "npc.h"
#include "game.h"
#include "level_sv.h"

class level_sv;

class npc_sv : public npc
{
    public:

        npc_sv(level_sv *, uint, vec *, vec *);
        virtual ~npc_sv();

        virtual void frame(double time_delta);

        virtual bool do_damage(uint target_actor, float damage);

    protected:
    private:
};

#endif // __NPC_SV_H__
