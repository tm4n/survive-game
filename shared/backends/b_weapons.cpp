#include "b_weapons.h"
#include "helper.h"
#include <sstream>

b_weapons::b_weapons()
{
	// init values
	
	// Colt 1911, WP_COLT
	
	wp_data[WP_COLT].name.assign("Colt M1911");
	#ifndef server
	wp_data[WP_COLT].wp_mesh = ResourceLoader::meshType::Colt;
	wp_data[WP_COLT].hand_mesh = ResourceLoader::meshType::Colt_hand;
	#endif
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
	
	#ifndef server
	wp_data[WP_COLT].snd_shoot = ResourceLoader::sndType::Colt_shot;
	wp_data[WP_COLT].snd_reload = ResourceLoader::sndType::Colt_reload;
	wp_data[WP_COLT].snd_reload_delay = 0.f;
	wp_data[WP_COLT].snd_bg_loop = ResourceLoader::sndType::None;
	#endif
	wp_data[WP_COLT].muzzle_pos_hand.set(36.f, -2.f, -6.f);
	wp_data[WP_COLT].muzzle_pos.set(6.0f, -0.3f, 2.0f);
	wp_data[WP_COLT].muzzle_size = 0.5f;
	
	// Chainsaw, WP_CHAINSAW
	
	wp_data[WP_CHAINSAW].name.assign("Chainsaw");
	#ifndef server
	wp_data[WP_CHAINSAW].wp_mesh = ResourceLoader::meshType::Chainsaw;
	wp_data[WP_CHAINSAW].hand_mesh = ResourceLoader::meshType::Chainsaw_hand;
	#endif
	wp_data[WP_CHAINSAW].wp_type = WP_TYPE_MEELE;
	wp_data[WP_CHAINSAW].damage = 4.f;
	wp_data[WP_CHAINSAW].magazin_size = 1;
	wp_data[WP_CHAINSAW].ammo_size = -2;
	wp_data[WP_CHAINSAW].bullets = 8;
	wp_data[WP_CHAINSAW].accuracy = 0.2f;
	wp_data[WP_CHAINSAW].shootspeed = 1.f;
	wp_data[WP_CHAINSAW].reloadspeed = 10.f;
	wp_data[WP_CHAINSAW].range = 55.f;
	wp_data[WP_CHAINSAW].recoil = 0.f;
	
	#ifndef server
	wp_data[WP_CHAINSAW].snd_shoot = ResourceLoader::sndType::Chainsaw_shot;
	wp_data[WP_CHAINSAW].snd_reload = ResourceLoader::sndType::None;
	wp_data[WP_CHAINSAW].snd_reload_delay = 0;
	wp_data[WP_CHAINSAW].snd_bg_loop = ResourceLoader::sndType::Chainsaw_loop;
	#endif
	wp_data[WP_CHAINSAW].muzzle_pos_hand.set(0.f, 0.f, 0.f);
	wp_data[WP_CHAINSAW].muzzle_pos.set(0.f, 0.f, 0.f);
	wp_data[WP_CHAINSAW].muzzle_size = 0;
	
	// Smith 'n Wesson, WP_WESSON
	
	wp_data[WP_WESSON].name.assign("Smith 'n Wesson");
	#ifndef server
	wp_data[WP_WESSON].wp_mesh = ResourceLoader::meshType::Wesson;
	wp_data[WP_WESSON].hand_mesh = ResourceLoader::meshType::Wesson_hand;
	#endif
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
	
	#ifndef server
	wp_data[WP_WESSON].snd_shoot = ResourceLoader::sndType::Wesson_shoot;
	wp_data[WP_WESSON].snd_reload = ResourceLoader::sndType::Colt_reload;
	wp_data[WP_WESSON].snd_reload_delay = 0.f;
	wp_data[WP_WESSON].snd_bg_loop = ResourceLoader::sndType::None;
	#endif
	wp_data[WP_WESSON].muzzle_pos_hand.set(39.2f, -6.4f, -5.f);
	wp_data[WP_WESSON].muzzle_pos.set(8.2f, 0.6f, 3.f);
	wp_data[WP_WESSON].muzzle_size = 0.6f;
	
	
	// H&K SL8, WP_HKSL8
	
	wp_data[WP_HKSL8].name.assign("H&K SL8");
	#ifndef server
	wp_data[WP_HKSL8].wp_mesh = ResourceLoader::meshType::HKSL8;
	wp_data[WP_HKSL8].hand_mesh = ResourceLoader::meshType::HKSL8_hand;
	#endif
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
	
	#ifndef server
	wp_data[WP_HKSL8].snd_shoot = ResourceLoader::sndType::HKSL8_shoot;
	wp_data[WP_HKSL8].snd_reload = ResourceLoader::sndType::Colt_reload;
	wp_data[WP_HKSL8].snd_reload_delay = -0.1f;
	wp_data[WP_HKSL8].snd_bg_loop = ResourceLoader::sndType::None;
	#endif
	wp_data[WP_HKSL8].muzzle_pos_hand.set(37.5f, -7.7f, -7.5f);
	wp_data[WP_HKSL8].muzzle_pos.set(23.8f, 0.f, 1.7f);
	wp_data[WP_HKSL8].muzzle_size = 1.f;
	
	
	
	// Shotgun, WP_SHOTGUN
	
	wp_data[WP_SHOTGUN].name.assign("Shotgun");
	#ifndef server
	wp_data[WP_SHOTGUN].wp_mesh = ResourceLoader::meshType::Shotgun;
	wp_data[WP_SHOTGUN].hand_mesh = ResourceLoader::meshType::Shotgun_hand;
	#endif
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
	
	#ifndef server
	wp_data[WP_SHOTGUN].snd_shoot = ResourceLoader::sndType::Shotgun_shoot;
	wp_data[WP_SHOTGUN].snd_reload = ResourceLoader::sndType::Shotgun_reload;
	wp_data[WP_SHOTGUN].snd_reload_delay = -0.3f;
	wp_data[WP_SHOTGUN].snd_bg_loop = ResourceLoader::sndType::None;
	#endif
	wp_data[WP_SHOTGUN].muzzle_pos_hand.set(34.4f, -7.f, -8.6f);
	wp_data[WP_SHOTGUN].muzzle_pos.set(16.5f, 0.f, 3.6f);
	wp_data[WP_SHOTGUN].muzzle_size = 1.f;
	
	
	// USAS 12, WP_USAS12
	
	wp_data[WP_USAS12].name.assign("USAS 12");
	#ifndef server
	wp_data[WP_USAS12].wp_mesh = ResourceLoader::meshType::USAS12;
	wp_data[WP_USAS12].hand_mesh = ResourceLoader::meshType::USAS12_hand;
	#endif
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
	
	#ifndef server
	wp_data[WP_USAS12].snd_shoot = ResourceLoader::sndType::USAS_shoot;
	wp_data[WP_USAS12].snd_reload = ResourceLoader::sndType::Colt_reload;
	wp_data[WP_USAS12].snd_reload_delay = -0.3f;
	wp_data[WP_USAS12].snd_bg_loop = ResourceLoader::sndType::None;
	#endif
	wp_data[WP_USAS12].muzzle_pos_hand.set(40.5f, -4.5f, -6.2f);
	wp_data[WP_USAS12].muzzle_pos.set(23.8f, 0.2f, 1.6f);
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
