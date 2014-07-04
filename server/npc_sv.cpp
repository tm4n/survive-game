#include "npc_sv.h"
#include "net_sv.h"
#include <sstream>

npc_sv::npc_sv(level_sv* lvl_sv, uint npc_type, vec *pos, vec *pan, int* counter) : npc(lvl_sv, npc_type, pos, pan)
{
	npc_target_timer = 0.f;
	send_pos_timer = 0.f;
	jump_timer = 0.f;
	sv_attack_done = false;
	old_target = target;
	last_position.zero();
	npc_counter = counter;
	// set events?
    // TODO: create AI depending on npc values

    std::ostringstream s;
    s << " created NPC, type=" << npc_type << "position.z = " << position.z;

    log(LOG_DEBUG_VERBOSE, s.str().c_str());

	// update to all players
    net_server->broadcast_sync_npc(id, npc_type, &position, &angle, health, target);
    
    *npc_counter += 1;
}

npc_sv::~npc_sv()
{
	net_server->broadcast_remove_actor(id);
	
	*npc_counter -= 1;
}

void npc_sv::frame(double time_delta)
{
	if (health > 0.f)
	{
		// AI
		//if (game_state != STATE_PLAYING) {enet_svent_remove(enet_ent_globpointer(me)); return;}
		
		
		npc_target_timer += (float)time_delta;
		
		if (target < 0 || (npc_target_timer >= 64.f && state != ST_ATTACK))
		{
			npc_target_timer = 0;
			
			// get new target
			if (get_ai_type() == NPC_AI_GENERATOR_DIRECT)
			{
			
				if (lvl->get_generator() >= 0) target = lvl->get_generator();
			}
			if (get_ai_type() == NPC_AI_PLAYER_DIRECT || get_ai_type() == NPC_AI_PLAYER_JUMP || get_ai_type() == NPC_AI_PLAYER_FLYING)
			{
				actor *z = get_nearest_target(false);
				if (z != NULL) target = z->id;
			}
			if (get_ai_type() == NPC_AI_BARRIERS_DIRECT)
			{
				actor *z = get_nearest_target(true);
				if (z == NULL) z = get_nearest_target(false);
				if (z != NULL) target = z->id;
			}
		}
		else
		{
			// do actual damage, only on server
			actor *t = lvl->actorlist.at(target);
			if (t == NULL) {target = 0;}
			else
			{
				if (t->health <= 0)  {target = 0;}
				else
				{
					if (state == ST_ATTACK)
					{
						if (attack_count < 30 && sv_attack_done == true) {sv_attack_done = false;}
						if (attack_count > 30 && sv_attack_done == false)
						{
							sv_attack_done = true;
							if (get_attack_type() == NPC_ATTACK_TYPE_MELEE)
							{
								t->health -= get_damage();
								if (t->health < 0)
								{
									t->health = 0; target = 0;
								}
								net_server->broadcast_update_health(t->id, t->health);
							}
							// else see npc()
						}
					}
				}
			}
		}
		
		// send unit updates
		// send target
		if (target != old_target)
		{
			old_target = target;
			net_server->broadcast_update_target(id, target);
		}
		
		// send position
		send_pos_timer += (float)time_delta;
		if (send_pos_timer >= SV_SEND_NPC_RATE)
		{
			send_pos_timer -= SV_SEND_NPC_RATE;
			if (position.dist(&last_position) > 0.01f)
			{
				last_position.set(&position);
				net_server->broadcast_update_pos(id, &position);
			}
		}
		
		// count down jump timer
		jump_timer -= (float)time_delta;
		jump_timer = std::max(jump_timer, 0.f);
	
		// call general functions
		movement(time_delta);
	}
	else
	{
		// TODO: dead, state will not be send
		//if (integer(random(30)) == 0 && my.ai_type != NPC_AI_PLAYER_FLYING) new collectible_sv(lvl, COLLECTIBLE_TYPE_HEALTH, &position);
		
	}

    state_manager(time_delta);
}


actor *npc_sv::get_nearest_target(bool get_barrier)
{
	float dist = 99999.f;
	float dist_crate = 99999.f;
	actor* best = NULL;
	actor* best_crate = NULL;
	
	for (uint i = 0; i < lvl->actorlist.size; i++)
	{
		actor *ac = lvl->actorlist.at(i);
		if (ac != NULL)
		{
			if (abs(position.z - ac->position.z) < get_ai_max_z_diff() && ac->health > 0.f)
			{
				if (ac->type == ACTOR_TYPE_BOX && position.dist(&ac->position) < dist_crate) 
				{
					dist_crate = position.dist(&ac->position);
					best_crate = ac;
				}
				if (ac->type == ACTOR_TYPE_PLAYER && position.dist(&ac->position) < dist)
				{
					dist = position.dist(&ac->position);
					best = ac;
				}
			}
		}
 	}
 	
 	if (get_barrier == false)
 	{
 		if (best != NULL) return(best); else return(best_crate);
 	}
 	else
 	{
 		if (best_crate != NULL) return(best_crate); else return(best);
 	}

	return (NULL);
}

void npc_sv::event_callback(int event_type, actor *ac)
{
	if (event_type == EVENT_TYPE_COLLISION_ENTITY && state != ST_ATTACK)
	{
		if ((get_ai_type() == NPC_AI_GENERATOR_DIRECT || get_ai_type() == NPC_AI_PLAYER_DIRECT || get_ai_type() == NPC_AI_PLAYER_FLYING || get_ai_type() == NPC_AI_BARRIERS_DIRECT) 
			&& (ac->faction == 1 || ac->type == ACTOR_TYPE_BOX))
		{
			target = ac->id;
			attack_count = 0.f;
		}
		
		if (get_ai_type() == NPC_AI_PLAYER_JUMP && jump_timer <= 0.f && state != ST_JUMPING && ac->type == ACTOR_TYPE_BOX)
		{
			npc_orders |= INPUT_JUMP;
			jump_timer = 16.f;
			net_server->broadcast_update_npc_orders(id, npc_orders);
		}
	}
}
