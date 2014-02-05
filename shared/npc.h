#ifndef __NPC_H__
#define __NPC_H__

#include "actor.h"
#include "backends/b_npcs.h"


class npc : public actor
{
    public:
        uint npc_type;
		int target;

        npc(level*, uint npc_type, vec *, vec *);
		npc(level*, uint actor_id, uint npc_type, vec *, vec *, float health, int target);
        virtual ~npc();

        void init_values();
    protected:
    private:
};

#endif // __NPC_H__
