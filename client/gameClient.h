#ifndef __GAMECLIENT_H__
#define __GAMECLIENT_H__

#include "game.h"
#include "gameRenderer.h"
#include "player_cl.h"

class gameClient : public game
{
public:
	ENetPeer *serverpeer;

	uint local_state;
	uint own_actor_id;

	gameClient(gameRenderer *arenderer);

	void handle_netevent(ENetEvent *event);

	bool connect(const char *ip, int port);
	void disconnect();

	void event_mouse(SDL_Event *evt);

	void frame(double time_delta);
	
	player_cl *get_own_player();

private:
	gameRenderer *renderer;

	float key_velx, key_vely;

};

#endif
