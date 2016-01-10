#ifndef __B_NPC_H__
#define __B_NPC_H__

#ifndef server
#include "ResourceLoader.h"
#endif // server

#include "defs.h"
#include <iostream>
#include <string>
#include <memory>
#include <mutex>

#define B_NPCS_ENTRIES 20


// NPC_TYPES
#define NPC_MUMMY 1
#define NPC_WEREWOLF 2
#define NPC_SMASHER 3
#define NPC_WITCH 4
#define NPC_HOGMAN 5
#define NPC_KNIGHT 6
#define NPC_BAUUL 7
#define NPC_HARPY 8

// NPC Data definitions
#define NPC_AI_GENERATOR_DIRECT 1
#define NPC_AI_PLAYER_DIRECT 2
#define NPC_AI_PLAYER_JUMP 3
#define NPC_AI_BARRIERS_DIRECT 4
#define NPC_AI_PLAYER_FLYING 5

#define NPC_ATTACK_TYPE_MELEE 0
#define NPC_ATTACK_TYPE_RANGED_FIREBALL 1


struct s_npcs{
	const char* name;
	#ifndef server
	ResourceLoader::meshType res_mesh;
	#endif
	float max_health;
	float move_speed;
	int attack_type;
	float attack_speed;
	float attack_range;
	float damage;
	int bounty;
	int ai_type;
	float ai_max_z_diff;
	float anim_speed;
	vec bb_min;
	vec bb_max;
	#ifndef server
	ResourceLoader::sndType res_snd_taunt1;
	ResourceLoader::sndType res_snd_taunt2;
	#endif
};

class b_npcs
{
public:
	b_npcs();

	s_npcs *at(uint id);

	static b_npcs *instance();

private:
	s_npcs npc_data[B_NPCS_ENTRIES];

	static std::unique_ptr<b_npcs> m_instance;
    static std::once_flag m_onceFlag;

	void err(const char *, int);
};

#endif
