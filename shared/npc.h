#ifndef __NPC_H__
#define __NPC_H__

#include <actor.h>

class npc : public actor
{
    public:
        uint npc_type;

        npc(level*, uint, vec *, vec *);
        virtual ~npc();

        void init_values();
    protected:
    private:
};

#endif // __NPC_H__
