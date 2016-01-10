#include "npc.h"
#include <backends/b_npcs.h>


npc::npc(level* lvl, uint npc_type, vec *pos, vec *pan) : actor(lvl, ACTOR_TYPE_NPC, pos, pan)
{
    this->npc_type = npc_type;
	this->health = b_npcs::instance()->at(npc_type)->max_health;
	this->target = -1;

	this->faction = 2;
	this->attack_count = 0.f;
	this->attack_done = false;
	this->npc_orders = 0;

	bb_max.set(&b_npcs::instance()->at(npc_type)->bb_max);
	bb_min.set(&b_npcs::instance()->at(npc_type)->bb_min);

	// put onto ground
	if (get_ai_type() != NPC_AI_PLAYER_FLYING)
	{
		vec v(0.f, 0.f, -1000.f);
		move_rel_col(&v);
	}
}

npc::npc(level* lvl, uint actor_id, uint npc_type, vec *pos, vec *pan, float health, int target) : actor(lvl, actor_id, ACTOR_TYPE_NPC, pos, pan)
{
    this->npc_type = npc_type;
	this->health = health;
	this->target = target;

	this->faction = 2;
	this->attack_count = 0.f;
	this->attack_done = false;
	this->npc_orders = 0;

	bb_max.set(&b_npcs::instance()->at(npc_type)->bb_max);
	bb_min.set(&b_npcs::instance()->at(npc_type)->bb_min);

	// put onto ground
	if (get_ai_type() != NPC_AI_PLAYER_FLYING)
	{
		vec v(0.f, 0.f, -1000.f);
		move_rel_col(&v);
	}
}

npc::~npc()
{
    //dtor
}


void npc::movement(double time_delta)
{
	vec v;
	if (health > 0.f)
	{
		// walk towards target
		if (target >= 0 && state != ST_FL_ASC)
		{
			actor *t = lvl->actorlist.at(target);
			if (t == NULL) { log(LOG_DEBUG, "UNSETTING TARGET, invalid\n"); target = -1; }
			else
			{
				if (t->health <= 0)
				{
					target = -1;
					log(LOG_DEBUG, "UNSETTING TARGET, no health\n");
				}
				else
				{
					if (position.dist(&t->position) > (get_attack_range()+bb_max.x) && state != ST_ATTACK)
					{
						if (npc_orders & NPC_ORDER_JUMP)
						{
							npc_orders &= ~NPC_ORDER_JUMP;
							state = ST_JUMPING;
							gravity += 20.f;
						}
						
						if (state != ST_JUMPING) state = ST_WALKING;
				
						// turn towards target
						turn_to(&t->position);
						angle.y = 0.f; angle.z = 0.f;	
						

						//and walk
						v.set(get_move_speed()*(float)time_delta,0.f,0.f);
						move_rel_col(&v);
					}
					else
					{
						
						// turn towards target
						turn_to(&t->position);
						angle.z = 0.f;
						
						if (get_ai_type() == NPC_AI_PLAYER_FLYING)
						{
							// FLYING ENEMY AI
							if (position.dist(&t->position) > 100.f && state != ST_ATTACK)
							{
								state = ST_FL_DESC;
								//and fly fast
								v.set(get_move_speed()*2.f*(float)time_delta,0.f,0.f);
								move_rel_col(&v);
							}
							else
							{
								// now we are ready to attack
								state = ST_ATTACK;
								
								// normal ground attack
								attack_count += get_attack_speed()*(float)time_delta;
								
								if (attack_count < 30.f && attack_done == true) {attack_done = false;}
								if (attack_count > 30.f && attack_done == false)
								{
									attack_done = true;
								}
		
								if (attack_count > 100.f) {attack_count = 0.f; state = ST_FL_DESC;}
							}
							
						}
						else
						{
							// NON FLYING ENEMY AI
							
							angle.y = 0.f; // don't tilt
							
							// now we are ready to attack
							state = ST_ATTACK;
							
							// normal ground attack
							attack_count += get_attack_speed()*(float)time_delta;
							
							if (attack_count < 30 && attack_done == true) {attack_done = false;}
							if (attack_count > 30 && attack_done == false)
							{
								attack_done = true;
								callback_attack_done(t->id, &t->position);
							}
	
							if (attack_count > 100.f) {attack_count = 0; state = ST_WALKING;}
						}
	
					}
				}
			}
		}
		
		if (get_ai_type() != NPC_AI_PLAYER_FLYING)
		{
			// gravity
			gravity -= actor_gravity*(float)time_delta;
			
			event_mask &=  ~ENABLE_EVENT_COLLISION_ENTITY;
			v.set(0, 0, gravity*(float)time_delta);
			float result = move_rel_col(&v);
			event_mask |= ENABLE_EVENT_COLLISION_ENTITY;
			if (result <= 0)
			{
				gravity = 0.f;
				if (state == ST_JUMPING)
				{
					state = ST_WALKING;
					// send my new position to client (wtf, run on both??!?!?!!)
					/*vec_set(my.skill[SK_TR_X], my.x);
					enet_send_skills(enet_ent_globpointer(me), SK_TR_X, SK_TR_Z, BROADCAST);*/
				}
			}
		}
		else
		{
			// ascending after attack
			if (state == ST_FL_ASC)
			{
				angle.y = 60.f;
			
				v.set(get_move_speed()*(float)time_delta,0,0);
				
				attack_count += (float)time_delta;
				if (attack_count > 100.f) {state = ST_WALKING; attack_count = 0; angle.y = 0;}
			}
		}
	}
	else
	{
		if (get_ai_type() == NPC_AI_PLAYER_FLYING)
		{
			if (state != ST_FL_FALLING && state != ST_DEATH) {state = ST_FL_FALLING; gravity = 0.;}

			if (state == ST_FL_FALLING)
			{
				// gravity
				gravity -= actor_gravity*(float)time_delta;

				event_mask &=  ~ENABLE_EVENT_COLLISION_ENTITY;
				v.set(0, 0, gravity*(float)time_delta);
				float result = move_rel_col(&v);
				event_mask |= ENABLE_EVENT_COLLISION_ENTITY;
				if (result <= 0)
				{
					// Hit the ground, dead!
					gravity = 0.f;
					state = ST_DEATH;
				}
			}
		}
		else
		{
			state = ST_DEATH;
		}
	}
		
}