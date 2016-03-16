#include "box_sv.h"
#include "net_sv.h"
#include "helper.h"
#include <algorithm>

box_sv::box_sv(level *lvl, char abox_type, vec *pos, int *counter)
: box(lvl, abox_type, pos)
{
	box_counter = counter;
	turret_target_timer = 0.;
	
	// set health
	if (abox_type == BOX_TYPE_WOOD) health = 100.f;
	if (abox_type == BOX_TYPE_METAL) health = 300.f;
	if (abox_type == BOX_TYPE_GENERATOR) health = 5.f; // default 500, DEBUG HERE
	if (abox_type == BOX_TYPE_TURRET) health = 50.f;

	// send creation to all connected players
	net_server->broadcast_sync_box(id, abox_type, pos, health, target);
	
	*box_counter += 1;
}

box_sv::~box_sv()
{
	// notify of removal
	net_server->broadcast_remove_actor(id);
	
	*box_counter -= 1;
}


void box_sv::frame(double time_delta)
{
	if (health <= 0.f)
	{
		delete this;
		return;
	}
		
	movement(time_delta);

	if (box_type == BOX_TYPE_TURRET)
	{
		// check for valid target entity
		if (target >= 0)
		{
			actor *ac = lvl->actorlist.at(target);
			if (ac == NULL) { log(LOG_DEBUG_VERBOSE, "TURRET LOST TARGET: didnt exist"); target = -1; net_server->broadcast_update_target(id, target); }
			else
			{
				if (ac->health <= 0.f || position.dist(&ac->position) > attack_range) { log(LOG_DEBUG_VERBOSE, "TURRET LOST TARGET: health or range");  target = -1; net_server->broadcast_update_target(id, target); }
				else
				{
					vec from(position.x, position.y, position.z + 22.f);
					if (asin(abs(position.z - ac->position.z) / position.dist(&ac->position)) >= toRadians(40.f)
						|| lvl->trace(from, ac->position, NULL, NULL, id, target) == true)
					{
						log(LOG_DEBUG_VERBOSE, "TURRET LOST TARGET: blocked or angle"); target = -1; net_server->broadcast_update_target(id, target);
					}
				}
			}
		}

		turret_target_timer += time_delta;

		if (target < 0 && turret_target_timer >= 32.)
		{
			turret_target_timer = 0.;

			// get new target
			float dist = 99999;
			actor *ac_nearest = NULL;
			for (uint i = 0; i < lvl->actorlist.size; i++) // repeat for all actors
			{
				actor *ac = lvl->actorlist.at(i);
				// check if its a possible candidate
				if (ac != NULL)
				{
					// is of right type
					if (ac->type == ACTOR_TYPE_NPC)
					{
							// is alive, within range and turret can turn to face him
						if (ac->health > 0 && position.dist(&ac->position) <= attack_range
							&& asin(abs(position.z - ac->position.z) / position.dist(&ac->position)) < toRadians(40.f))
						{
							// no obstacle is in the way
							vec from(position.x, position.y, position.z + 22.f);
							if (position.dist(&ac->position) < dist && lvl->trace(from, ac->position, NULL, NULL, id, i) == false)
							{
								dist = position.dist(&ac->position);
								ac_nearest = ac;
							}
						}
					}
				}
			}


			if (ac_nearest != NULL)
			{
				log(LOG_DEBUG_VERBOSE, "TURRET FOUND TARGET");
				target = ac_nearest->id;
				net_server->broadcast_update_target(id, target);
			}
		}

		// continous attack
		attack_count += (double)attack_speed*time_delta;

		// attack target
		if (target >= 0)
		{
			actor *t = lvl->actorlist.at(target);
			if (t != NULL)
			{
				if (attack_count > 100.)
				{
				
					t->health -= 0.6f; // damage target
					if (t->health <= 0.f)
					{
						t->health = 0.f; t->target = 0;
					}
					net_server->broadcast_update_health(t->id, t->health);

					attack_count = 0.;
				}
			}
		}
	}
}
