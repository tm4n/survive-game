#include "collectible_sv.h"
#include "net_sv.h"
#include <sstream>

collectible_sv::collectible_sv(level *lvl, char acollectible_type, vec *pos)
	: collectible(lvl, collectible_type, pos)
{
	std::ostringstream s;
    s << " created collectible, actor_id = " << id <<
		", type=\"" << collectible_type << "\"";

    log(LOG_DEBUG_VERBOSE, s.str().c_str());
    
    net_server->broadcast_sync_collectible(id, collectible_type, pos);
}

collectible_sv::~collectible_sv()
{
	net_server->broadcast_remove_actor(id);
}