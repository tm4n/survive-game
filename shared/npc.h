#ifndef __NPC_H__
#define __NPC_H__

#include <actor.h>

#define NPC_LOOTER_ID_NONE -1
#define NPC_LOOTER_ID_NOLOOT -2

class npc : public actor
{
    public:
        uint npc_type;

        int looter_id;

        npc(level*, uint, vec *, vec *);
        virtual ~npc();

        virtual float calc_move_speed();

        void init_values();
    protected:
    private:
};

#endif // __NPC_H__
