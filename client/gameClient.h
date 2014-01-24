#ifndef __GAMECLIENT_H__
#define __GAMECLIENT_H__

#include "game.h"
#include "gameRenderer.h"
#include "player_cl.h"
#include "net_cl.h"
#include "level_cl.h"

#define CAMERA_VIEW_HEIGHT 1

class gameClient : public game
{
public:

	bool input_enable;
	uint local_state;
	uint own_actor_id;
	
	level_cl *lvl_cl;

	gameClient(gameRenderer *arenderer);
	~gameClient();

	void handle_netevent(ENetEvent *event);

	bool connect(const char *ip, int port);
	bool connect(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);
	void disconnect();

	void event_mouse(SDL_Event *evt);

	void frame(double time_delta);
	
	player_cl *get_own_player();

private:
	gameRenderer *renderer;

	int input;
	float cam_bob_offset;

};

#endif
