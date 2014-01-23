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

// enum defines
#define LOG_DEBUG_VERBOSE 1
#define LOG_DEBUG 2
#define LOG_WARNING 3
#define LOG_ERROR 4

class level_sv;



class gameServer : public game
{
public:
    static const int FRAMES_PER_SECOND = 30;

    bool quit; // Quit flag
    long frame; // number of frames already rendered
    
    flist<ENetPeer> peers;

    level_sv *lvl_sv;

    gameServer();
    gameServer(std::list<ENetPacket*> *in_queue, std::mutex *mutex_in_queue, std::list<ENetPacket*> *out_queue, std::mutex *mutex_out_queue);
    virtual ~gameServer();
    
    void run();

    void synchronizeClient(ENetPeer *receiver);
	void handle_netevent(ENetEvent *event);
	
	void start_match();

	void log(int, const char *);
	
	
	int sv_spawned_npcs;  // number of spawned npcs this round
	int sv_amount_npcs;  // number of npcs to spawn this round
	int sv_num_npcs;  // number of npcs currently alive

	int sv_num_barriers;

	float sv_spawn_timer;
	float sv_spawn_cap;
	float sv_barrier_timer;

private:
    bool init();
    
    void next_wave();
    
    void clean_up();
};

#endif // __GAMESERVER_H__
