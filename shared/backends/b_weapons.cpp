#include "b_weapons.h"
#include "helper.h"
#include <sstream>

b_weapons::b_weapons()
{
	// init values
	
	// Colt 1911, WP_COLT
	
	wp_data[WP_COLT].name.assign("Colt M1911");
	//wp_data[WP_COLT].model = str_create("colt.mdl");
	//wp_data[WP_COLT].handmodel = str_create("colt_hand.mdl");
	wp_data[WP_COLT].wp_type = WP_TYPE_BULLET;
	wp_data[WP_COLT].damage = 4.f;
	wp_data[WP_COLT].magazin_size = 7;
	wp_data[WP_COLT].ammo_size = -1;
	wp_data[WP_COLT].bullets = 1;
	wp_data[WP_COLT].accuracy = 1.09f;
	wp_data[WP_COLT].shootspeed = 2.0f;
	wp_data[WP_COLT].reloadspeed = 0.22f;
	wp_data[WP_COLT].range = 5000.f;
	wp_data[WP_COLT].recoil = 0.6f;
	
	//wp_data[WP_COLT].snd_shoot = snd_create("colt_shot.wav");
	//wp_data[WP_COLT].snd_reload = snd_create("colt_reload.wav");
	wp_data[WP_COLT].snd_reload_delay = 0.f;
	wp_data[WP_COLT].muzzle_pos.set(36.f, -2.f, -6.f);
	wp_data[WP_COLT].muzzle_size = 0.5f;
	
	// Chainsaw, WP_CHAINSAW
	
	wp_data[WP_CHAINSAW].name.assign("Chainsaw");
	//wp_data[WP_CHAINSAW].model = str_create("chainsaw.mdl");
	//wp_data[WP_CHAINSAW].handmodel = str_create("chainsaw_hands.mdl");
	wp_data[WP_CHAINSAW].wp_type = WP_TYPE_MEELE;
	wp_data[WP_CHAINSAW].damage = 4.f;
	wp_data[WP_CHAINSAW].magazin_size = 1;
	wp_data[WP_CHAINSAW].ammo_size = -1;
	wp_data[WP_CHAINSAW].bullets = 8;
	wp_data[WP_CHAINSAW].accuracy = 0.2f;
	wp_data[WP_CHAINSAW].shootspeed = 1.f;
	wp_data[WP_CHAINSAW].reloadspeed = 10.f;
	wp_data[WP_CHAINSAW].range = 55.f;
	wp_data[WP_CHAINSAW].recoil = 0.f;
	
	//wp_data[WP_CHAINSAW].snd_shoot = snd_create("chainsaw_shot.wav");
	//wp_data[WP_CHAINSAW].snd_reload = NULL;
	wp_data[WP_CHAINSAW].snd_reload_delay = 0;
	//wp_data[WP_CHAINSAW].snd_bg_loop = snd_create("chainsaw_loop.wav");
	wp_data[WP_CHAINSAW].muzzle_pos.set(0.f, 0.f, 0.f);
	wp_data[WP_CHAINSAW].muzzle_size = 0;
	
		// Smith 'n Wesson, WP_WESSON
	
	wp_data[WP_WESSON].name.assign("Smith 'n Wesson");
	//wp_data[WP_WESSON].model = str_create("wesson.mdl");
	//wp_data[WP_WESSON].handmodel = str_create("wesson_hand.mdl");
	wp_data[WP_WESSON].wp_type = WP_TYPE_BULLET;
	wp_data[WP_WESSON].damage = 12.f;
	wp_data[WP_WESSON].magazin_size = 6;
	wp_data[WP_WESSON].ammo_size = 24;
	wp_data[WP_WESSON].bullets = 1;
	wp_data[WP_WESSON].accuracy = 0.7f;
	wp_data[WP_WESSON].shootspeed = 1.5f;
	wp_data[WP_WESSON].reloadspeed = 0.14f;
	wp_data[WP_WESSON].range = 5000.f;
	wp_data[WP_WESSON].recoil = 2.0f;
	
	//wp_data[WP_WESSON].snd_shoot = snd_create("wesson_shot.wav");
	//wp_data[WP_WESSON].snd_reload = snd_create("colt_reload.wav");
	wp_data[WP_WESSON].snd_reload_delay = 0.f;
	wp_data[WP_WESSON].muzzle_pos.set(39.2f, -6.4f, -5.f);
	wp_data[WP_WESSON].muzzle_size = 0.6f;
	
	
	// H&K SL8, WP_HKSL8
	
	wp_data[WP_HKSL8].name.assign("H&K SL8");
	//wp_data[WP_HKSL8].model = str_create("hksl8.mdl");
	//wp_data[WP_HKSL8].handmodel = str_create("hksl8_hand.mdl");
	wp_data[WP_HKSL8].wp_type = WP_TYPE_BULLET;
	wp_data[WP_HKSL8].damage = 4.f;
	wp_data[WP_HKSL8].magazin_size = 30;
	wp_data[WP_HKSL8].ammo_size = 90;
	wp_data[WP_HKSL8].bullets = 1;
	wp_data[WP_HKSL8].accuracy = 2.5f;
	wp_data[WP_HKSL8].shootspeed = 4.5f;
	wp_data[WP_HKSL8].reloadspeed = 0.28f;
	wp_data[WP_HKSL8].range = 5000.f;
	wp_data[WP_HKSL8].recoil = 0.9f;
	
	//wp_data[WP_HKSL8].snd_shoot = snd_create("hksl8_shot.wav");
	//wp_data[WP_HKSL8].snd_reload = snd_create("colt_reload.wav");
	wp_data[WP_HKSL8].snd_reload_delay = -0.1f;
	wp_data[WP_HKSL8].muzzle_pos.set(37.5f, -7.7f, -7.5f);
	wp_data[WP_HKSL8].muzzle_size = 1.f;
	
	
	
	// Shotgun, WP_SHOTGUN
	
	wp_data[WP_SHOTGUN].name.assign("Shotgun");
	//wp_data[WP_SHOTGUN].model = str_create("shotgun.mdl");
	//wp_data[WP_SHOTGUN].handmodel = str_create("shotgun_hand.mdl");
	wp_data[WP_SHOTGUN].wp_type = WP_TYPE_BULLET;
	wp_data[WP_SHOTGUN].damage = 4.5f;
	wp_data[WP_SHOTGUN].magazin_size = 2;
	wp_data[WP_SHOTGUN].ammo_size = 14;
	wp_data[WP_SHOTGUN].bullets = 8;
	wp_data[WP_SHOTGUN].accuracy = 6.f;
	wp_data[WP_SHOTGUN].shootspeed = 1.8f;
	wp_data[WP_SHOTGUN].reloadspeed = 0.33f;
	wp_data[WP_SHOTGUN].range = 5000.f;
	wp_data[WP_SHOTGUN].recoil = 2.5f;
	
	//wp_data[WP_SHOTGUN].snd_shoot = snd_create("shotgun_shot.wav");
	//wp_data[WP_SHOTGUN].snd_reload = snd_create("shotgun_reload.wav");
	wp_data[WP_SHOTGUN].snd_reload_delay = -0.3f;
	wp_data[WP_SHOTGUN].muzzle_pos.set(34.4f, -7.f, -8.6f);
	wp_data[WP_SHOTGUN].muzzle_size = 1.f;
	
	
	// USAS 12, WP_USAS12
	
	wp_data[WP_USAS12].name.assign("USAS 12");
	//wp_data[WP_USAS12].model = str_create("usas12.mdl");
	//wp_data[WP_USAS12].handmodel = str_create("usas12_hand.mdl");
	wp_data[WP_USAS12].wp_type = WP_TYPE_BULLET;
	wp_data[WP_USAS12].damage = 3.8f;
	wp_data[WP_USAS12].magazin_size = 50;
	wp_data[WP_USAS12].ammo_size = 150;
	wp_data[WP_USAS12].bullets = 1;
	wp_data[WP_USAS12].accuracy = 3.f;
	wp_data[WP_USAS12].shootspeed = 5.0f;
	wp_data[WP_USAS12].reloadspeed = 0.2f;
	wp_data[WP_USAS12].range = 5000.f;
	wp_data[WP_USAS12].recoil = 1.4f;
	
	//wp_data[WP_USAS12].snd_shoot = snd_create("usas12_shot.wav");
	//wp_data[WP_USAS12].snd_reload = snd_create("colt_reload.wav");
	wp_data[WP_USAS12].snd_reload_delay = -0.3f;
	wp_data[WP_USAS12].muzzle_pos.set(40.5f, -4.5f, -6.2f);
	wp_data[WP_USAS12].muzzle_size = 1.f;

}

s_weapons *b_weapons::at(uint id)
{
	if (id > B_WEAPONS_ENTRIES) {err("b_weapons::at was called with invalid id", -1); return NULL;}
	return &wp_data[id];
}

void b_weapons::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_weapons in id=" << id << " :" << msg;

	log(LOG_ERROR, pr.str().c_str());
}

// Thread safe singleton!
std::unique_ptr<b_weapons> b_weapons::m_instance;
std::once_flag b_weapons::m_onceFlag;

b_weapons * b_weapons::instance()
{
    std::call_once(m_onceFlag,
        [] {
            m_instance.reset(new b_weapons);
    });
    return m_instance.get();
}
