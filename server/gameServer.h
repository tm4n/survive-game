#ifndef __GAMESERVER_H__
#define __GAMESERVER_H__

#include <game.h>

//Headers and includes
#include "enet/enet.h"

#include "level_sv.h"

#include <string>
#include <list>
#include <mutex>
#include <set>

class level_sv;



class gameServer : public game
{
public:
    static const int FRAMES_PER_SECOND = 60;

    bool quit; // Quit flag
    long frame; // number of frames already rendered
    
    //flist<ENetPeer> peers;

    level_sv *lvl_sv;

    gameServer();
    gameServer(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);
    virtual ~gameServer();
    
    void run();

    void synchronizeClient(ENetPeer *receiver);
	void handle_netevent(ENetEvent *event);
	
	int get_num_players();
	
	void reset();
	void start_match();
	void spawner(double time_frame);
	
	void npc_spawn(int etype, float ebonus);
	void box_spawn();
	void wpdrop_spawn(int wtype);
	
	// settings: number of max. enemies
	int sv_num_npcs_limit;
	float sv_barrier_probability;
	
	// internal counters:
	int sv_spawned_npcs;  // number of spawned npcs this round
	int sv_amount_npcs;  // number of npcs to spawn this round
	int sv_num_npcs;  // number of npcs currently alive

	int sv_num_barriers;

	float sv_spawn_timer;
	float sv_spawn_cap;
	float sv_barrier_timer;
	float wave_wait_tick;
	
	float sv_wave_bonus;

private:
    bool init();
    
    void next_wave();
    
    void clean_up();
};

#endif // __GAMESERVER_H__
