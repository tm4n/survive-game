#include "player_cl.h"
#include "box_cl.h"
#include "net_cl.h"
#include "helper.h"

player_cl::player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int weapon, int input, int object_taken, gameRenderer *arenderer)
	: player(lvl, actor_id, pos, ang, health, name, weapon, input, object_taken)
{
	renderer = arenderer;
	local_player = false;

	ro = new RenderObject();
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
        	
    renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(ro);

	send_pos_timer = 0.f;
	send_angle_timer = 0.f;
}


player_cl::~player_cl()
{
	renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->removeRenderObject(ro);

	delete ro;
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
						// smaller is better
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
	
	movement((float)time_delta);

	// only for local player: send my position to the server:
	if (local_player == true)
	{
		send_pos_timer += (float)time_delta;
			
		if (send_pos_timer >= SV_SEND_PLAYER_RATE/2.f)
		{
			send_pos_timer -= SV_SEND_PLAYER_RATE/2.f;

			net_client->send_update_pos(id, &position, net_client->serverpeer);
		}
	}
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z;
}

