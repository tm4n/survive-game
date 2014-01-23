#include "box_sv.h"
#include "net_sv.h"

box_sv::box_sv(level *lvl, char abox_type, vec *pos, float ahealth)
: box(lvl, abox_type, pos, ahealth)
{
	//lvl_sv = lvl;
	// send creation to all connected players
	net_server->broadcast_sync_box(id, abox_type, pos, ahealth);
}

box_sv::~box_sv()
{
	// notify of removal
	net_server->broadcast_remove_actor(id);
}
