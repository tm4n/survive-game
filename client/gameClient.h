#ifndef __GAMECLIENT_H__
#define __GAMECLIENT_H__

#include "game.h"
#include "gameRenderer.h"
#include "player_cl.h"
#include "net_cl.h"
#include "level_cl.h"
#include "gui_hud.h"
#include "effectmgr.h"

class gameClient : public game
{
public:

	bool input_enable;
	uint local_state;
	uint own_actor_id;
	float respawn_timer;

	uint points;
	
	level_cl *lvl_cl;

	gameClient(gameRenderer *arenderer);
	~gameClient();

	void reset();

	void handle_netevent(ENetEvent *event);

	bool connect(const char *ip, int port);
	bool connect(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);
	void disconnect();

	void event_input(SDL_Event *evt);

	void frame(double time_delta);
	
	player_cl *get_own_player();

private:
	gameRenderer *renderer;
	effectmgr *effmgr;

	int input;
	float cam_bob_offset, cam_bob_prog;

	gui_hud *hud;

};

#endif
