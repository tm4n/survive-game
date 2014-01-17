#include "box_sv.h"
#include "net_sv.h"

box_sv::box_sv(level *lvl, vec *pos, uint ahealth)
: box(lvl, pos, ahealth)
{
	//lvl_sv = lvl;
	// send creation to all connected players
	net_broadcast_sync_box(id, pos, ahealth);
}

box_sv::~box_sv()
{
	// notify of removal
	net_broadcast_remove_actor(id);
}
