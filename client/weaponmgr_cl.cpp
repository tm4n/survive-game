#include "weaponmgr_cl.h"

#include "helper.h"
#include "net_cl.h"

weaponmgr_cl::weaponmgr_cl(level *lvl, int *curr_weapon, bool *local_player, gameRenderer *renderer, int player_id)
	: weaponmgr(lvl, curr_weapon)
{
	this->local_player = local_player;
	this->renderer = renderer;
	this->player_id = player_id;
	this->anim_state = 0;
	this->anim_count = 0.f;

	s_weapons *wdata = b_weapons::instance()->at(1);
	ro = new RenderObject();
	ro->translation[0] = 5;
    ro->translation[1] = -2;
	ro->translation[2] = -74;
        	
	getMesh(*curr_weapon)->addRenderObject(ro);
}

weaponmgr_cl::~weaponmgr_cl()
{
	delete ro;
}

void weaponmgr_cl::input_shoot(vec &cam_pos, vec &cam_angle)
{
	if (wp_ready == false || get_curr_magazin() <= 0)  return;

	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);

	wp_ready = 0;
	wp_cooldown += 10.f;

	// get target
	vec v;
	v.set(&cam_pos);
	
	v.x += (float) (cos(toRadians(cam_angle.x))*cos(toRadians(cam_angle.y))) * wdata->range;
	v.y += (float) (sin(toRadians(cam_angle.x))*cos(toRadians(cam_angle.y))) * wdata->range;
	v.z += (float) (sin(toRadians(cam_angle.y))) * wdata->range;


	// send to server
	net_client->send_shoot(player_id, &v, 0, net_client->serverpeer);

	set_anim_state(1);
	// TODO: shoot-sound
	// TODO: muzzle flash effect

}

int weaponmgr_cl::input_switch(int num)
{
	if (num < 0 || num > WEAPON_ENTRIES) {return(0);}
	if (!(pickups & (1<<num)) || *curr_weapon == num) {return(0);}
	//if (player.health <= 0 || player.client_keys & INPUT_SPRINT || player.skill[SK_TR_WP_CHANGE] != 0) {return(0);}

	log(LOG_DEBUG, "Client sending weapon switch!");
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
	if (*local_player == false) return; // TODO: hold ro in hand

	// count down cooldown
	if (wp_cooldown > 0.f && wp_switching == 0)
	{
		wp_cooldown -= b_weapons::instance()->at(*curr_weapon)->shootspeed*(float)time_frame;
		if (wp_cooldown <= 0) wp_ready = true;
	}



	// display weapon

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


	// animate weapon
	// shoot
	if (anim_state == 1)
	{
		getMesh(*curr_weapon)->animate(ro, "fire", anim_count, 0);
		anim_count += b_weapons::instance()->at(*curr_weapon)->shootspeed*10.f*(float)time_frame;
		if (anim_count >= 100.f) set_anim_state(0);
	}
	if (anim_state == 2)
	{
		if (anim_count <= 50.f)
		{
			getMesh(*curr_weapon)->animate(ro, "arm", 100.f-(anim_count*2.f), 0);
			//if (wp_pl_loop_handle != 0) snd_tune(wp_pl_loop_handle, 100-(anim_count*2), 0, 0);
		}
		if (anim_count > 50.f)
		{
			if (wp_cooldown > 0.f)
			{
				wp_cooldown = 0.f;

				getMesh(*curr_weapon)->removeRenderObject(ro);
				getMesh(wp_switching)->addRenderObject(ro);
				
				// sound
				/*if (wp_pl_loop_handle != 0) snd_stop(wp_pl_loop_handle);
				if (wp_data[num].snd_bg_loop != NULL) wp_pl_loop_handle = snd_loop(wp_data[num].snd_bg_loop, 0, 0);
				else wp_pl_loop_handle = 0;*/
			}
			if (wp_switching != 0)	getMesh(wp_switching)->animate(ro, "arm",  ((anim_count-50.f)*2.f), 0);
				else getMesh(*curr_weapon)->animate(ro, "arm",  ((anim_count-50.f)*2.f), 0);
			//if (wp_pl_loop_handle != 0) snd_tune(wp_pl_loop_handle, (anim_count-50)*2, 0, 0);
		}
		
		anim_count += (float)time_frame*10.f;
		if (anim_count >= 100.f) {set_anim_state(0);}
	}
}

void weaponmgr_cl::set_anim_state(int new_state)
{
	anim_count = 0.f;

	if (anim_state == 0)
	{
		getMesh(*curr_weapon)->animate(ro, "fire", 0.f, 0);
	}

	anim_state = new_state;
}

void weaponmgr_cl::set_mag_ammo(int weapon_id, short mag, short am)
{
	if (magazin[weapon_id] == 0 && ammo[weapon_id] == 0 && !(pickups & (1 << weapon_id)))
	{
		pickups |= (1 << weapon_id);

		//snd_pickup();
		//show_message("You've picked up a new weapon!\nChange weapons with mouse wheel or number keys.");
	}

	magazin[weapon_id] = mag;
	ammo[weapon_id] = am;

	std::cout << "client received ammo update: " << magazin[weapon_id] << " " << ammo[weapon_id] << std::endl;
}

void weaponmgr_cl::switch_cl(int new_weapon_id)
{
	wp_ready = false;
	wp_switching = new_weapon_id;
	wp_cooldown = 10.f;

	set_anim_state(2);
}

Mesh *weaponmgr_cl::getMesh(int weapon_id)
{
	return renderer->resources.getMesh(b_weapons::instance()->at(weapon_id)->hand_mesh);
}