#include "player_cl.h"
#include "box_cl.h"
#include "net_cl.h"
#include "helper.h"
#include <algorithm>

player_cl::player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int weapon, int input, int object_taken, gameRenderer *arenderer, effectmgr *effmgr)
	: player(lvl, actor_id, pos, ang, health, name, weapon, input, object_taken)
{
	renderer = arenderer;
	local_player = false;
	input_shoot = false;

	ro = new RenderObject();

	wpmgr = new weaponmgr_cl(lvl, &curr_weapon, &local_player, &state, renderer, effmgr, id);
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
        	
    renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(ro);

	send_pos_timer = 0.f;
	send_angle_timer = 0.f;
	step_count = 0.;
	step_first = false;
}


player_cl::~player_cl()
{
	renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->removeRenderObject(ro);

	delete ro;
	delete wpmgr;
}



void player_cl::order_take_object()
{
	if (object_taken < 0)
	{
		// scan for crates
		std::list<uint> res;
		lvl->get_actors_within(&res, &position, 100.);

		float shortest_ang = 360.f;
		actor *nearest_ac = NULL;
		for (uint it : res)
		{
			actor *ac = lvl->actorlist.at(it);
			if (ac)
			{
				if (ac->type == ACTOR_TYPE_BOX)
				{
					// calculate angle from me to you
					vec v (ac->position.x - position.x, ac->position.y - position.y, (ac->position.z + ac->bb_max.z/2.f) - position.z);
					float ang_pan = angle.x - vec::angle(90.f - vec::angle(atan2(v.x, v.y)*(float)(180.0/M_PI)));
					ang_pan = abs(vec::angle(ang_pan));

					float ang_tilt = angle.y - vec::angle(asin(v.z / v.length())*(float)(180.0/M_PI));
					ang_tilt = abs(vec::angle(ang_tilt));

					if (ang_pan < 50.f && ang_tilt < 50.f)
					{
						// smaller is better TODO: fix!
						float heur = ang_pan + ang_tilt*2.f + position.dist(&ac->position)/20.f;
						if (shortest_ang > heur)
						{
							shortest_ang = heur;
							nearest_ac = ac;
						}
					}
				}
			}
		}
		// send
		if (nearest_ac != NULL) net_client->send_take(id, nearest_ac->id, net_client->serverpeer);
	}
	else
	{
		//send that we want to drop the crate
		net_client->send_take(id, -1, net_client->serverpeer);
	}
}


void player_cl::frame(double time_delta)
{
	if (health <= 0.f) printf("0 health player!");

	// shooting
	if (wpmgr->wp_ready == true && input_shoot == true)
	{
		vec pos(renderer->CameraPos.x, renderer->CameraPos.y, renderer->CameraPos.z);
		vec ang(renderer->CameraAngle.x, renderer->CameraAngle.y, renderer->CameraAngle.z);
		if (health > 0 && !(input & INPUT_SPRINT) && object_taken == -1) wpmgr->input_shoot(pos, ang);
	}

	// reloading
	if (wpmgr->get_curr_magazin() == 0 && wpmgr->get_curr_ammo() != 0 && wpmgr->wp_ready == true && wpmgr->wp_reloading == 0
		&& !(input & INPUT_SPRINT) && object_taken == -1)
	{
		wpmgr->input_reload();
	}
	
	movement((float)time_delta);

	// only for local player: send my position +angle to the server:
	if (local_player == true)
	{
		// turn player with camera
		angle.x = renderer->CameraAngle.x;

		// send regularly
		send_angle_timer += (float)time_delta;
		if (send_angle_timer >= CL_SEND_ANGLE_RATE) 
		{
			send_angle_timer -= CL_SEND_ANGLE_RATE;
			//if (old_pan != player.skill[SK_TR_PAN] || player.skill[SK_TR_PAN_DIR] != 0) 
			//{  Problem with pan_dir causing the server to have a different pan
			net_client->send_update_ang(id, angle.x, 0.f, net_client->serverpeer);
			//}
		}

		send_pos_timer += (float)time_delta;
			
		if (send_pos_timer >= SV_SEND_PLAYER_RATE/2.f)
		{
			send_pos_timer -= SV_SEND_PLAYER_RATE/2.f;

			net_client->send_update_pos(id, &position, net_client->serverpeer);
		}
	}
	else
	{
		animate(time_delta);
	}

	wpmgr->frame(time_delta, position, angle, ro->animFrame);
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z;

	// walk sound
	if (state == ST_WALKING || state == ST_WALKING_LEFT || state == ST_WALKING_RIGHT)
	{
		if (step_count > 0. && step_first == false) { snd_ent_step(false); step_first = true; }
		if (step_count > 100.) { snd_ent_step(true); step_count = -100.; step_first = false; }

		if (input & INPUT_SPRINT) step_count += move_speed*2.5*time_delta; else step_count += move_speed*2*time_delta;

	}
	else { step_count = 0.; step_first = false; }
}

void player_cl::animate(double time_delta)
{
	if (old_anim != state) { anim_prog = 0; old_anim = state; }

	float anim_speed;
	if (input & INPUT_SPRINT) anim_speed = 1.4f; else anim_speed = 0.9f;
	float move_speed = 9.0f;
	Mesh *mesh = renderer->resources.getMesh(ResourceLoader::meshType::Soldier);

	if (state == ST_IDLE)
	{
		if (object_taken == -1) mesh->animate(ro, "stand", anim_prog, 1); else mesh->animate(ro, "crate_stand", anim_prog, 1);
		anim_prog += move_speed / 4.f*(float)time_delta*anim_speed;
	}

	if (state == ST_WALKING)
	{
		if (object_taken == -1) mesh->animate(ro, "run", anim_prog, 1); else mesh->animate(ro, "crate_run", anim_prog, 1);
		anim_prog += move_speed*1.2f*(float)time_delta*anim_speed;
	}

	if (state == ST_WALKING_LEFT)
	{
		if (object_taken == -1) mesh->animate(ro, "left_run", anim_prog, 1); else mesh->animate(ro, "crate_left_run", anim_prog, 1);
		anim_prog += move_speed*1.2f*(float)time_delta*anim_speed;
	}

	if (state == ST_WALKING_RIGHT)
	{
		if (object_taken == -1) mesh->animate(ro, "right_run", anim_prog, 1); else mesh->animate(ro, "crate_right_run", anim_prog, 1);
		anim_prog += move_speed*1.2f*(float)time_delta*anim_speed;
	}

	if (state == ST_JUMPING)
	{
		mesh->animate(ro, "jump", anim_prog, 0);
		anim_prog += move_speed*2.f*(float)time_delta*anim_speed;
	}

	if (state == ST_DEATH)
	{
		anim_prog = std::min(anim_prog, 100.f);
		mesh->animate(ro, "death", anim_prog, 0);
		return;
	}

	if (anim_prog > 100.f && (state == ST_WALKING || state == ST_IDLE)) { anim_prog -= 100.f; }
}

void player_cl::snd_ent_step(bool dir)
{
	Sound *snd_step = renderer->resources.getSnd(ResourceLoader::sndType::Step);
	if (local_player == true)
	{
		if (dir == false) { snd_step->play(1, 50.f, 40.f); }
		else { snd_step->play(1, 50.f, -40.f); }
	}
	else { snd_step->play3D(1, ro, 20.f); }
}