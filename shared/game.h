#ifndef __GAME_H__
#define __GAME_H__

#include "defs.h"
#include "enet/enet.h"
#include "level.h"
#include <vector>


#define GAME_STATE_WAITING 1
#define GAME_STATE_RUNNING 2
#define GAME_STATE_END 3

class game
{

public:
	game();

    level *lvl;

	int state;
	
	int wave;
	
	int wait_timer;


	virtual void handle_netevent(ENetEvent *event) = 0;
};


#endif
