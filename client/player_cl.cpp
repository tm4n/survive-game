#include "player_cl.h"
#include "net_cl.h"

player_cl::player_cl(level *lvl, uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int weapon, int input, int object_taken, gameRenderer *arenderer)
	: player(lvl, actor_id, pos, ang, health, name, weapon, input, object_taken)
{
	renderer = arenderer;

	ro = new RenderObject();
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
        	
    renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(ro);
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
		net_client->send_take(id, 1, net_client->serverpeer);
	}
	else
	{
		net_client->send_take(id, -1, net_client->serverpeer);
	}
}


void player_cl::frame(double time_delta)
{
	// turn player with camera
	angle.x = renderer->CameraAngle.x;
	
	// send regularly
	send_angle_count += time_delta;
	if (send_angle_count >= CL_SEND_ANGLE_RATE) 
	{
		send_angle_count -= CL_SEND_ANGLE_RATE;
		//if (old_pan != player.skill[SK_TR_PAN] || player.skill[SK_TR_PAN_DIR] != 0) 
		//{  Problem with pan_dir causing the server to have a different pan
		net_client->send_update_ang(id, angle.x, 0.f, net_client->serverpeer);
		//}
	}
	
	movement(time_delta);
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z;
}

