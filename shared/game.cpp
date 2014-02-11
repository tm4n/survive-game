#include "game.h"

game::game()
{
	state = GAME_STATE_WAITING;
	wave = 0;
	
	lvl = NULL;
	
	wave_wait_timer = 0;
}

game::~game()
{
	
}
