#ifndef __GAMESERVER_H__
#define __GAMESERVER_H__

#include <game.h>

//Headers and includes
#include "enet/enet.h"

#include "level_sv.h"
#include "SimpleConnectionPool.h"

#include "backends/b_skills.h"

#include <string>
#include <list>
#include <set>

// enum defines
#define LOG_DEBUG_VERBOSE 1
#define LOG_DEBUG 2
#define LOG_WARNING 3
#define LOG_ERROR 4

#define QUERY_LOGIN 1
#define QUERY_REGISTER 2
#define QUERY_SAVE 3
#define QUERY_ZONES 4
#define QUERY_SAVE_LAST_LOGIN 5

class level_sv;



class gameServer : public game
{
public:
    static const int FRAMES_PER_SECOND = 30;

    bool quit; // Quit flag
    long frame; // number of frames already rendered

    level_sv *lvl_sv;

    gameServer(bool);

    void synchronizeClient(ENetPeer *receiver);
	void handle_netevent(ENetEvent *event);

	void log(int, const char *);

private:
    bool init();
    void clean_up();
};

#endif // __GAMESERVER_H__
