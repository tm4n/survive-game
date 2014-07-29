#include "weaponmgr_cl.h"

#include "actor.h"
#include "helper.h"
#include "net_cl.h"

weaponmgr_cl::weaponmgr_cl(level *lvl, int *curr_weapon, bool *local_player, ushort *plstate, gameRenderer *renderer, effectmgr *effmgr, int player_id)
	: weaponmgr(lvl, curr_weapon)
{
	this->local_player = local_player;
	this->plstate = plstate;
	this->renderer = renderer;
	this->effmgr = effmgr;
	this->player_id = player_id;
	this->anim_state = 0;
	this->anim_count = 0.f;

	// displayed weapon

	//s_weapons *wdata = b_weapons::instance()->at(1);
	ro = new RenderObject();
	ro->translation[0] = 5;
    ro->translation[1] = -2;
	ro->translation[2] = -74;
        	
	this->curr_mesh = getMesh(*curr_weapon);
	this->curr_mesh->addRenderObject(ro);

	// muzzle flash
	ro_mf = new RenderObject();
	ro_mf->visible = false;
	ro_mf->alpha = 0.f;

	renderer->resources.getMesh(ResourceLoader::meshType::Muzzleflash)->addRenderObject(ro_mf);
}

weaponmgr_cl::~weaponmgr_cl()
{
	this->curr_mesh->removeRenderObject(ro);
	delete ro;

	renderer->resources.getMesh(ResourceLoader::meshType::Muzzleflash)->removeRenderObject(ro_mf);
	delete ro_mf;
}

void weaponmgr_cl::input_shoot(vec &cam_pos, vec &cam_angle)
{
	if (wp_ready == false || get_curr_magazin() <= 0)  return;

	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);

	// reduce ammo if weapon doesn't have unlimited amount
	if (wdata->ammo_size != -2) magazin[*curr_weapon] -= 1;

	wp_ready = false;
	wp_cooldown += 10.f;

	// get target
	vec v;
	v.set(&cam_pos);
	
	v.x += (float) (cos(toRadians(cam_angle.x))*cos(toRadians(cam_angle.y))) * wdata->range;
	v.y += (float) (sin(toRadians(cam_angle.x))*cos(toRadians(cam_angle.y))) * wdata->range;
	v.z += (float) (sin(toRadians(cam_angle.y))) * wdata->range;


	// send to server
	net_client->send_shoot(player_id, &v, 0, net_client->serverpeer);

	recoil.y = wdata->recoil*(2.f+random_range(1));
	recoil.x = wdata->recoil*(0.5f + random_range(1.5f));
	recoil.z = random_range(2); // pan negative or positive

	set_anim_state(1);
	// TODO: shoot-sound
	show_muzzleflash(*curr_weapon);

}

int weaponmgr_cl::input_switch(int num)
{
	if (wp_ready == false || wp_switching != 0 || hidden == true) return 0;
	if (num < 0 || num > WEAPON_ENTRIES) return(0);
	if (!(pickups & (1<<num)) || *curr_weapon == num) return(0);

	log(LOG_DEBUG, "Client sending weapon switch!");
	net_client->send_change_weapon(player_id, num, net_client->serverpeer);
	wp_switching = -1;
	return(1);
}

void weaponmgr_cl::input_scroll_up()
{
	if (wp_ready == false || wp_switching != 0 || hidden == true) return;

	int curr = *curr_weapon;

	while (curr < WEAPON_ENTRIES)
	{
		curr += 1;
		if (input_switch(curr) == 1) {return;}
	}
}

void weaponmgr_cl::input_scroll_down()
{
	if (wp_ready == false || wp_switching != 0 || hidden == true) return;

	int curr = *curr_weapon;

	while (curr > 0)
	{
		curr -= 1;
		if (input_switch(curr) == 1) {return;}
	}
}

void weaponmgr_cl::input_reload()
{
	if (wp_ready == false || get_curr_ammo() == 0
		|| get_curr_magazin() >= b_weapons::instance()->at(*curr_weapon)->magazin_size)  return;

	net_client->send_reload(player_id, net_client->serverpeer);

	reload();
}

void weaponmgr_cl::frame(double time_frame)
{
	if (*local_player == false) return; // TODO: hold ro in hand

	float time_frame_float = (float)time_frame;

	// count down cooldown
	if (wp_cooldown > 0.f && wp_switching == 0)
	{
		wp_cooldown -= b_weapons::instance()->at(*curr_weapon)->shootspeed*time_frame_float;
		if (wp_cooldown <= 0) wp_ready = true;
	}


	// display weapon

	ro->translation[0] = renderer->CameraPos.x;
    ro->translation[1] = renderer->CameraPos.y;
	ro->translation[2] = renderer->CameraPos.z;

	move_dir(ro->translation, renderer->CameraAngle, 5.f, -2.f, -74.f);

	ro->rotation[0] = renderer->CameraAngle.x;
	ro->rotation[1] = -renderer->CameraAngle.y;
	ro->rotation[2] = renderer->CameraAngle.z;


	// muzzle flash
	ro_mf->translation[0] = renderer->CameraPos.x;
	ro_mf->translation[1] = renderer->CameraPos.y;
	ro_mf->translation[2] = renderer->CameraPos.z;

	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);
	move_dir(ro_mf->translation, renderer->CameraAngle, wdata->muzzle_pos.x, wdata->muzzle_pos.y, wdata->muzzle_pos.z);

	ro_mf->rotation[0] = renderer->CameraAngle.x;
	ro_mf->rotation[1] = -renderer->CameraAngle.y;
	//ro_mf->rotation[2] = renderer->CameraAngle.z;

	if (ro_mf->alpha > 0.1f)
	{
		ro_mf->alpha -= 0.45f*time_frame_float;
		if (ro_mf->alpha <= 0.1f) ro_mf->visible = false;
	}


	// animate weapon
	if (anim_state == 0)
	{
		if ((*plstate == ST_WALKING || *plstate == ST_WALKING_LEFT || *plstate == ST_WALKING_RIGHT) && wp_ready == true && wp_reloading == 0)
		{
			curr_mesh->animate(ro, "bob", anim_count, 1);
			anim_count += 9.f * time_frame_float;
			if (anim_count > 100.f) anim_count -= 100.f;
		}
		else
		{
			if (anim_count != 0.f)
			{
				anim_count = 0.f;
				curr_mesh->animate(ro, "bob", anim_count, 1);
			}
		}
	}
	// shoot
	if (anim_state == 1)
	{
		curr_mesh->animate(ro, "fire", anim_count, 0);
		anim_count += b_weapons::instance()->at(*curr_weapon)->shootspeed*10.f*time_frame_float;
		if (anim_count >= 100.f) set_anim_state(0);

		// apply recoil
		if (recoil.y > 0)
		{
			recoil.y = std::max(0.f, recoil.y -time_frame_float*3.f);
			renderer->CameraAngle.y += recoil.y*time_frame_float;
		}
		if (recoil.x > 0)
		{
			recoil.x = std::max(0.f, recoil.x -time_frame_float*3.f);
			if (recoil.z < 1.f) renderer->CameraAngle.x += recoil.x*time_frame_float; else renderer->CameraAngle.x -= recoil.x*time_frame_float;
		}
	}
	if (anim_state == 2)
	{
		if (anim_count <= 50.f)
		{
			curr_mesh->animate(ro, "arm", 100.f-(anim_count*2.f), 0);
			//if (wp_pl_loop_handle != 0) snd_tune(wp_pl_loop_handle, 100-(anim_count*2), 0, 0);
		}
		if (anim_count > 50.f)
		{
			if (wp_cooldown > 0.f)
			{
				wp_cooldown = 0.f;

				curr_mesh->removeRenderObject(ro);
				curr_mesh = getMesh(wp_switching);
				curr_mesh->addRenderObject(ro);
				
				// sound
				/*if (wp_pl_loop_handle != 0) snd_stop(wp_pl_loop_handle);
				if (wp_data[num].snd_bg_loop != NULL) wp_pl_loop_handle = snd_loop(wp_data[num].snd_bg_loop, 0, 0);
				else wp_pl_loop_handle = 0;*/
			}
			curr_mesh->animate(ro, "arm",  ((anim_count-50.f)*2.f), 0);
			//if (wp_pl_loop_handle != 0) snd_tune(wp_pl_loop_handle, (anim_count-50)*2, 0, 0);
		}
		
		anim_count += time_frame_float*10.f;
		if (anim_count >= 100.f) {set_anim_state(0);}
	}
	if (anim_state == 3)
	{
		curr_mesh->animate(ro, "reload", anim_count, 0);
		anim_count += b_weapons::instance()->at(*curr_weapon)->reloadspeed*10.f*time_frame_float;
		if (anim_count >= 100.f)
		{
			set_anim_state(0);
		}
	}

	if (anim_state == 4)
	{
		// hide
		curr_mesh->animate(ro, "arm", 100.f-anim_count, 0);
		anim_count += 9.f*4.f*time_frame_float;
		anim_count = std::min(anim_count, 100.f);
	}
	if (anim_state == 5)
	{
		// unhide
		curr_mesh->animate(ro, "arm", anim_count, 0);
		anim_count += 9.f*4.f*time_frame_float;
		if (anim_count >= 100.f) {set_anim_state(0);}
	}
}

void weaponmgr_cl::hide_wp()
{
	if (hidden == true) return;
	hidden = true;
	set_anim_state(4);
	//ro->visible = false;
}

void weaponmgr_cl::show_wp()
{
	if (hidden == false) return;
	hidden = false;
	set_anim_state(5);
	//ro->visible = true;
}

void weaponmgr_cl::set_anim_state(int new_state)
{
	anim_count = 0.f;

	if (anim_state == 0 || new_state == 0)
	{
		curr_mesh->animate(ro, "bob", 1.f, 0);
	}

	anim_state = new_state;
}

void weaponmgr_cl::set_mag_ammo(int weapon_id, short mag, short am)
{
	if (magazin[weapon_id] == 0 && ammo[weapon_id] == 0 && !(pickups & (1 << weapon_id)))
	{
		pickups |= (1 << weapon_id);

		//snd_pickup();
	}
	else
	{
		if (wp_reloading != 0)
		{
			// done with reloading
			wp_reloading = 0;
			wp_ready = true;
		}
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

void weaponmgr_cl::update_curr_weapon(int new_curr_weapon)
{
	*curr_weapon = new_curr_weapon;
	wp_ready = true;
	wp_switching = 0;

	if (getMesh(new_curr_weapon) != curr_mesh) 
	{
		// switch weapon mesh
		curr_mesh->removeRenderObject(ro);
		curr_mesh = getMesh(new_curr_weapon);
		curr_mesh->addRenderObject(ro);
	}
}

MeshGUI *weaponmgr_cl::getMesh(int weapon_id)
{
	return dynamic_cast<MeshGUI*> (renderer->resources.getMesh(b_weapons::instance()->at(weapon_id)->hand_mesh));
}

void weaponmgr_cl::shoot(vec &shoot_origin, vec &shoot_dir, int rnd_seed)
{
	vec shoot_target;
	
	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);

	int shoot_nums = wdata->bullets;

	random_seed(rnd_seed);

	while (shoot_nums > 0)
	{
		shoot_nums--;

		shoot_target.set(&shoot_dir);
		shoot_target.x += (random_range(10)-5.f) * 10.f * wdata->accuracy;
		shoot_target.y += (random_range(10)-5.f) * 10.f * wdata->accuracy;
		shoot_target.z += (random_range(10)-5.f) * 10.f * wdata->accuracy;

		/*std::ostringstream s;
		s << "Shooting between " << shoot_origin << " and " << shoot_target;
		log(LOG_DEBUG_VERBOSE, s.str().c_str());*/

		vec hitpos;
		int actor_hit = 0;
		if (lvl->trace(player_id, shoot_origin, shoot_target, &hitpos, &actor_hit))
		{
			if (actor_hit >= 0) // hit someone
			{
				actor *ac = lvl->actorlist.at(actor_hit);
				if (ac->faction == 2) // hit a zombie
				{
					// TODO: blood
				}
				else
				{
					vec v(0, 0, 0);
					effmgr->eff_wallhit(&hitpos, &v);
				}
			}
			else
			{
				vec v(0, 0, 0);
				effmgr->eff_wallhit(&hitpos, &v);
			}
		}

		if (*local_player == true)
		{
			vec orig;
			orig.x = ro_mf->translation[0]; orig.y = ro_mf->translation[1]; orig.z = ro_mf->translation[2];

			effmgr->eff_bullettrail(&orig, &shoot_target);
		}
		else
		{
			vec orig;
			orig.set(&shoot_origin);
			orig.z -= 7.f;
			effmgr->eff_bullettrail(&orig, &shoot_target);
		}
	
	}
}

void weaponmgr_cl::reload()
{
	wp_ready = false;
	wp_reloading = 1;
	set_anim_state(3);
}

void weaponmgr_cl::cancel_reload()
{
	if (wp_reloading == 0) return;
	wp_reloading = 0;
	wp_ready = true;
	set_anim_state(0);
}

void weaponmgr_cl::show_muzzleflash(int weapon_id)
{
	s_weapons *wdata = b_weapons::instance()->at(*curr_weapon);

	if (wdata->muzzle_size <= 0.f) return;
	
	ro_mf->scale[0] = wdata->muzzle_size;
	ro_mf->scale[1] = wdata->muzzle_size;
	ro_mf->scale[2] = wdata->muzzle_size;

	/*pl_muzzle.lightrange = 50;
	pl_muzzle.green = 255; pl_muzzle.blue = 200; pl_muzzle.red = 255;*/

	ro_mf->alpha = 0.8f;
	ro_mf->rotation[2] = random_range(360.f);
	ro_mf->visible = true;
}