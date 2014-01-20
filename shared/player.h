#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "actor.h"

//#define INPUT_FIRE	(1<<0) done with packet
#define INPUT_JUMP	(1<<1)
//#define INPUT_TAKE	(1<<2) done with packet
#define INPUT_FORW	(1<<3)
#define INPUT_BACK	(1<<4)
#define INPUT_LEFT	(1<<5)
#define INPUT_RIGHT	(1<<6)
#define INPUT_SPRINT (1<<7)
#define INPUT_RELOAD (1<<10)


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
