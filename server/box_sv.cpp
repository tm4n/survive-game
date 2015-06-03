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

	// send creation to all connected players
	net_server->broadcast_sync_box(id, abox_type, pos, health);
	
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
		/*if (my.target_ent != 0)
		{
			ptr_temp = enet_ent_locpointer(my.target_ent - 1);
			if (ptr_temp == NULL) { sv_debug("TURRET LOST TARGET: didnt exist"); my.target_ent = 0; enet_send_skills(enet_ent_globpointer(me), SK_TARGET_ENT, SK_TARGET_ENT, BROADCAST); }
			else
			{
				if (ptr_temp.health <= 0 || vec_dist(ptr_temp.x, my.x) > my.attack_range) { sv_debug("TURRET LOST TARGET: health or range"); my.target_ent = 0; enet_send_skills(enet_ent_globpointer(me), SK_TARGET_ENT, SK_TARGET_ENT, BROADCAST); }
				else
				{
					you = ptr_temp;
					if (asinv(abs(my.z - ptr_temp.z) / vec_dist(my.x, ptr_temp.x)) >= 40 || c_trace(vector(my.x, my.y, my.z + 22), you.x, IGNORE_ME | IGNORE_YOU | IGNORE_PASSABLE | IGNORE_SPRITES) != 0)
					{
						sv_debug("TURRET LOST TARGET: blocked or angle"); my.target_ent = 0; enet_send_skills(enet_ent_globpointer(me), SK_TARGET_ENT, SK_TARGET_ENT, BROADCAST);
					}
					you = NULL;
				}
			}
		}

		npc_target_timer += time_frame;

		if (my.target_ent == 0 && my.npc_target_timer >= 32)
		{
			my.npc_target_timer = 0;

			// get new target

			dist = 99999;
			ent = NULL;
			ptr_temp = ent_next(NULL); // retrieve first entity
			while (ptr_temp) // repeat until there are no more entities
			{
				// check if its a possible candidate
				if (ptr_temp.type_id == TYPE_ID_NPC)
				{
					if (asinv(abs(my.z - ptr_temp.z) / vec_dist(my.x, ptr_temp.x)) < 40 && ptr_temp.health > 0 && vec_dist(my.x, ptr_temp.x) <= my.attack_range)
					{
						you = ptr_temp;
						if (vec_dist(my.x, ptr_temp.x) < dist && c_trace(vector(my.x, my.y, my.z + 22), you.x, IGNORE_ME | IGNORE_YOU | IGNORE_PASSABLE | IGNORE_SPRITES) == 0)
						{
							dist = vec_dist(my.x, ptr_temp.x);
							ent = ptr_temp;
						}
						you = NULL;
					}
				}
				ptr_temp = ent_next(ptr_temp); // get next entity
			}


			if (ent != NULL)
			{
				sv_debug("TURRET FOUND TARGET");
				my.target_ent = enet_ent_globpointer(ent) + 1;
				enet_send_skills(enet_ent_globpointer(me), SK_TARGET_ENT, SK_TARGET_ENT, BROADCAST);
			}
		}*/

		// continous attack
		/*my.attack_count += my.attack_speed*time_step;

		// attack target
		if (my.target_ent != 0)
		{
			ptr_temp = enet_ent_locpointer(my.target_ent - 1);
			if (ptr_temp != NULL)
			{

				if (my.attack_count > 100)
				{
				
					ptr_temp.health -= 0.6; // damage target
					ptr_temp.health = maxv(0, ptr_temp.health);

					my.attack_count = 0;
				}
			}
		}
		else
		{
			
		}*/
	}
}
