#ifndef __GAMECLIENT_H__
#define __GAMECLIENT_H__

#include "game.h"
#include "gameRenderer.h"

class gameClient : public game
{
public:
	ENetPeer *serverpeer;


	gameClient(gameRenderer *arenderer);

	void handle_netevent(ENetEvent *event);

	bool connect(const char *ip, int port);
	void disconnect();

	void frame(double time_delta);

private:
	gameRenderer *renderer;
};

#endif