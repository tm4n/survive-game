#include "game.h"

game::game()
{
	state = GAME_STATE_WAITING;
	wave = 0;
	
	wait_timer = 0;
}
