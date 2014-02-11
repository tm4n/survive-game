#include "box_sv.h"
#include "net_sv.h"

box_sv::box_sv(level *lvl, char abox_type, vec *pos, int *counter)
: box(lvl, abox_type, pos, 100.f)
{
	box_counter = counter;
	
	// set health
	if (abox_type == BOX_TYPE_WOOD) health = 100.f;
	if (abox_type == BOX_TYPE_METAL) health = 300.f;
	if (abox_type == BOX_TYPE_GENERATOR) health = 500.f;
	if (abox_type == BOX_TYPE_TURRET) health = 50.f;
	old_health = health;

	// send creation to all connected players
	net_server->broadcast_sync_box(id, abox_type, pos, health);
	
	box_counter += 1;
}

box_sv::~box_sv()
{
	// notify of removal
	net_server->broadcast_remove_actor(id);
	
	box_counter -= 1;
}


void box_sv::frame(double time_delta)
{
	if (health <= 0.f)
	{
		delete this;
		return;
	}

	if (old_health != health)
	{
		old_health = health;
		net_server->broadcast_update_health(id, health);
	}
		
	movement(time_delta);
}
