#include "player_sv.h"
#include "net_sv.h"

#include <sstream>



player_sv::player_sv(level *lvl, vec *pos, vec *ang, float health,
               const char *name, ENetPeer *owner)
               : player(lvl, pos, ang, health, name)
{

    this->owner = owner;


    std::ostringstream s;
    s << " created Player, actor_id = " << id <<
        ", name=\"" << name << "\"";

    log(LOG_DEBUG_VERBOSE, s.str().c_str());
    
    net_broadcast_sync_player(id, pos, ang, health, name, state, input);
}

player_sv::~player_sv()
{
    log(LOG_DEBUG_VERBOSE, "player_sv: deleting player");
}
