#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "actor.h"

class player : public actor
{
    public:

        int input;

        char name[32];
		


        player(level *, vec *pos, vec *pan, float health,
               const char *name);
		player(level *, 
			   uint actor_id, vec *pos, vec *pan, float health,
               const char *name, int state, int input);
        virtual ~player();

    protected:
    private:
};

#endif // __PLAYER_H__
