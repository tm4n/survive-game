#ifndef __B_NPC_H__
#define __B_NPC_H__

#include "ResourceLoader.h"
#include "defs.h"
#include <iostream>
#include <string>
#include <memory>
#include <mutex>

#define B_WEAPONS_ENTRIES 20


// WEAPON TYPES
#define WP_COLT 1
#define WP_CHAINSAW 2
#define WP_WESSON 3
#define WP_HKSL8 4
#define WP_SHOTGUN 5
#define WP_USAS12 6
//...

#define WP_TYPE_BULLET 1
#define WP_TYPE_PROJECTILE 2
#define WP_TYPE_MEELE 3


struct s_weapons{
	std::string name;
	ResourceLoader::meshType wp_mesh;
	ResourceLoader::meshType hand_mesh;
	int wp_type;
	float damage;
	int magazin_size;
	int ammo_size;
	int bullets; //number of bullets in one shot
	float accuracy;
	float shootspeed;
	float reloadspeed;
	float range;
	float recoil;
	
	// effect 
	//SOUND* snd_shoot;
	//SOUND* snd_reload;
	float snd_reload_delay;
	//SOUND* snd_bg_loop;
	
	vec muzzle_pos;
	float muzzle_size;
};

class b_weapons
{
public:
	b_weapons();

	s_weapons *at(uint id);

	static b_weapons *instance();

private:
	s_weapons wp_data[B_WEAPONS_ENTRIES];

	static std::unique_ptr<b_weapons> m_instance;
    static std::once_flag m_onceFlag;

	void err(const char *, int);
};

#endif
