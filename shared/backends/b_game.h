#ifndef __B_GAME_H__
#define __B_GAME_H__

#include "defs.h"
#include <string>

class b_game
{
public:
	std::string game_name;
	std::string login_ip_address;
	int login_port;
	// TODO: move more game settings from defines
	
	b_game(const char *);

private:
	void err(const char *);
};


// globally available instance
extern b_game *b_ga;

#endif
