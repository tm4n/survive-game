#ifndef __GAME_H__
#define __GAME_H__

#include "defs.h"
#include "enet/enet.h"
#include "backends/b_classes.h"
#include "backends/b_npcs.h"
#include "backends/b_items.h"
#include "level.h"
#include "actor.h"
#include <vector>


class game
{

public:
	ENetHost *Ehost;

    level *lvl;

	uint zone_id;


	virtual void handle_netevent(ENetEvent *event) = 0;
};


#endif
