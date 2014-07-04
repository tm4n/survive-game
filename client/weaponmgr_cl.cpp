#include "weaponmgr_cl.h"

#include "helper.h"
#include "net_cl.h"

weaponmgr_cl::weaponmgr_cl(level *lvl, int *curr_weapon, gameRenderer *renderer, int player_id)
	: weaponmgr(lvl, curr_weapon)
{
	this->renderer = renderer;
	this->player_id = player_id;

	s_weapons *wdata = b_weapons::instance()->at(1);
	ro = new RenderObject();
	ro->translation[0] = 5;
    ro->translation[1] = -2;
	ro->translation[2] = -74;
        	
	renderer->resources.getMesh(ResourceLoader::meshType::Colt_hand)->addRenderObject(ro);
}

void weaponmgr_cl::input_shoot(vec &cam_pos, vec &cam_angle)
{
	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);

	wp_ready = 0;
	wp_cooldown += 10;

	// get target
	vec v;
	v.set(&cam_pos);
	
	v.x += (float) (cos(toRadians(cam_angle.x))*cos(toRadians(cam_angle.y))) * wdata->range;
	v.y += (float) (sin(toRadians(cam_angle.x))*cos(toRadians(cam_angle.y))) * wdata->range;
	v.z += (float) (sin(toRadians(cam_angle.y))) * wdata->range;

	/*vec_set(player.shoot_dir, vector(screen_size.x/2, screen_size.y/2, wp_data[player.weapon].range));
	vec_for_screen (player.shoot_dir, camera);
	if (player.shoot_dir == 0) {player.shoot_dir = 0.001;}*/

	// send to server
	net_client->send_shoot(player_id, &v, 0, net_client->serverpeer);
	
	/*gui_shoot_animation();
	
	while (player)
	{
		if (player.wp_cooldown <= 0) break;
		
		player.wp_cooldown -= wp_data[player.weapon].shootspeed*time_step;
		wait(1);
	}
	
	if (player) player.wp_ready = 1; */
}

int weaponmgr_cl::input_switch(int num)
{
	if (num < 0 || num > WEAPON_ENTRIES) {return(0);}
	if (!(pickups & (1<<num)) || *curr_weapon == num) {return(0);}
	//if (player.health <= 0 || player.client_keys & INPUT_SPRINT || player.skill[SK_TR_WP_CHANGE] != 0) {return(0);}

	net_client->send_change_weapon(player_id, num, net_client->serverpeer);
	return(1);
}

void weaponmgr_cl::input_scroll_up()
{
	int curr = *curr_weapon;

	while (curr < WEAPON_ENTRIES)
	{
		curr += 1;
		if (input_switch(curr) == 1) {return;}
	}
}

void weaponmgr_cl::input_scroll_down()
{
	int curr = *curr_weapon;

	while (curr > 0)
	{
		curr -= 1;
		if (input_switch(curr) == 1) {return;}
	}
}

void weaponmgr_cl::frame(double time_frame)
{
	ro->translation[0] = renderer->CameraPos.x;
    ro->translation[1] = renderer->CameraPos.y;
	ro->translation[2] = renderer->CameraPos.z;

	// move by x
	ro->translation[0] += (float) (cos(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * 5.f;
	ro->translation[1] += (float) (sin(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y))) * 5.f;
	ro->translation[2] += (float) (sin(toRadians(renderer->CameraAngle.y))) * 5.f;

	// move by y
	ro->translation[0] += (float) (cos(toRadians(renderer->CameraAngle.x-90.f))) * 2.f;
	ro->translation[1] += (float) (sin(toRadians(renderer->CameraAngle.x-90.f))) * 2.f;

	// move by z
	ro->translation[0] += (float) (cos(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y-90.f))) * 74.f;
	ro->translation[1] += (float) (sin(toRadians(renderer->CameraAngle.x))*cos(toRadians(renderer->CameraAngle.y-90.f))) * 74.f;
	ro->translation[2] += (float) (sin(toRadians(renderer->CameraAngle.y-90.f))) * 74.f;

	/*ro->translation[0] = renderer->CameraPos.x; // + 5.f;
    ro->translation[1] = renderer->CameraPos.y; //- 2.f;
	ro->translation[2] = renderer->CameraPos.z - 74.f;*/

	ro->rotation[0] = renderer->CameraAngle.x;
	ro->rotation[1] = -renderer->CameraAngle.y;
	ro->rotation[2] = renderer->CameraAngle.z;
}