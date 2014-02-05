#include "b_npcs.h"
#include "helper.h"
#include <sstream>

b_npcs *b_np;

b_npcs::b_npcs(const char *filename)
{
	// init values
		// NPC_MUMMY
	npc_data[NPC_MUMMY].name = "Mummy";
	npc_data[NPC_MUMMY].res_mesh = ResourceLoader::meshType::Mummy;
	npc_data[NPC_MUMMY].max_health = 8;
	npc_data[NPC_MUMMY].move_speed = 5;
	npc_data[NPC_MUMMY].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_MUMMY].attack_speed = 4;
	npc_data[NPC_MUMMY].attack_range = 60;
	npc_data[NPC_MUMMY].damage = 10;
	npc_data[NPC_MUMMY].bounty = 20;
	npc_data[NPC_MUMMY].ai_type = NPC_AI_PLAYER_DIRECT;
	npc_data[NPC_MUMMY].ai_max_z_diff = 80;
	npc_data[NPC_MUMMY].anim_speed = 1;
	//npc_data[NPC_MUMMY].snd_taunt1 = snd_create("zombie1.wav");
	//npc_data[NPC_MUMMY].snd_taunt2 = snd_create("zombie2.wav");
	
	
	// NPC_WEREWOLF
	npc_data[NPC_WEREWOLF].name = "Werewolf";
	npc_data[NPC_WEREWOLF].res_mesh = ResourceLoader::meshType::Werewolf;
	npc_data[NPC_WEREWOLF].max_health = 9;
	npc_data[NPC_WEREWOLF].move_speed = 20;
	npc_data[NPC_WEREWOLF].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_WEREWOLF].attack_speed = 5;
	npc_data[NPC_WEREWOLF].attack_range = 60;
	npc_data[NPC_WEREWOLF].damage = 8;
	npc_data[NPC_WEREWOLF].bounty = 70;
	npc_data[NPC_WEREWOLF].ai_type = NPC_AI_PLAYER_JUMP;
	npc_data[NPC_WEREWOLF].ai_max_z_diff = 99999;
	npc_data[NPC_WEREWOLF].anim_speed = 0.6f;
	
	// NPC_SMASHER
	npc_data[NPC_SMASHER].name = "Smasher";
	npc_data[NPC_SMASHER].res_mesh = ResourceLoader::meshType::Smasher;
	npc_data[NPC_SMASHER].max_health = 25;
	npc_data[NPC_SMASHER].move_speed = 5;
	npc_data[NPC_SMASHER].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_SMASHER].attack_speed = 4;
	npc_data[NPC_SMASHER].attack_range = 65;
	npc_data[NPC_SMASHER].damage = 35;
	npc_data[NPC_SMASHER].bounty = 50;
	npc_data[NPC_SMASHER].ai_type = NPC_AI_BARRIERS_DIRECT;
	npc_data[NPC_SMASHER].ai_max_z_diff = 100;
	npc_data[NPC_SMASHER].anim_speed = 1.f;
	
	// NPC_WITCH
	npc_data[NPC_WITCH].name = "Witch";
	npc_data[NPC_WITCH].res_mesh = ResourceLoader::meshType::Witch;
	npc_data[NPC_WITCH].max_health = 10;
	npc_data[NPC_WITCH].move_speed = 7;
	npc_data[NPC_WITCH].attack_type = NPC_ATTACK_TYPE_RANGED_FIREBALL;
	npc_data[NPC_WITCH].attack_speed = 1.7f;
	npc_data[NPC_WITCH].attack_range = 1000;
	npc_data[NPC_WITCH].damage = 10;
	npc_data[NPC_WITCH].bounty = 30;
	npc_data[NPC_WITCH].ai_type = NPC_AI_PLAYER_DIRECT;
	npc_data[NPC_WITCH].ai_max_z_diff = 99999;
	npc_data[NPC_WITCH].anim_speed = 1;
	//npc_data[NPC_WITCH].npc_attach = str_create("witch_stick.mdl");
	
	// NPC_HOGMAN
	npc_data[NPC_HOGMAN].name = "Hogman";
	npc_data[NPC_HOGMAN].res_mesh = ResourceLoader::meshType::Hogman;
	npc_data[NPC_HOGMAN].max_health = 30;
	npc_data[NPC_HOGMAN].move_speed = 9;
	npc_data[NPC_HOGMAN].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_HOGMAN].attack_speed = 4;
	npc_data[NPC_HOGMAN].attack_range = 65;
	npc_data[NPC_HOGMAN].damage = 25;
	npc_data[NPC_HOGMAN].bounty = 50;
	npc_data[NPC_HOGMAN].ai_type = NPC_AI_PLAYER_DIRECT;
	npc_data[NPC_HOGMAN].ai_max_z_diff = 80;
	npc_data[NPC_HOGMAN].anim_speed = 1;
	//npc_data[NPC_HOGMAN].snd_taunt1 = snd_create("monster.wav");
	
	// NPC_KNIGHT
	npc_data[NPC_KNIGHT].name = "Dark Knight";
	npc_data[NPC_KNIGHT].res_mesh = ResourceLoader::meshType::Knight;
	npc_data[NPC_KNIGHT].max_health = 18;
	npc_data[NPC_KNIGHT].move_speed = 9;
	npc_data[NPC_KNIGHT].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_KNIGHT].attack_speed = 6;
	npc_data[NPC_KNIGHT].attack_range = 60;
	npc_data[NPC_KNIGHT].damage = 20;
	npc_data[NPC_KNIGHT].bounty = 30;
	npc_data[NPC_KNIGHT].ai_type = NPC_AI_GENERATOR_DIRECT;
	npc_data[NPC_KNIGHT].ai_max_z_diff = 80;
	npc_data[NPC_KNIGHT].anim_speed = 1;
	
	// NPC_BAUUL
	npc_data[NPC_BAUUL].name = "Bauul";
	npc_data[NPC_BAUUL].res_mesh = ResourceLoader::meshType::Bauul;
	npc_data[NPC_BAUUL].max_health = 13;
	npc_data[NPC_BAUUL].move_speed = 25;
	npc_data[NPC_BAUUL].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_BAUUL].attack_speed = 5;
	npc_data[NPC_BAUUL].attack_range = 60;
	npc_data[NPC_BAUUL].damage = 15;
	npc_data[NPC_BAUUL].bounty = 90;
	npc_data[NPC_BAUUL].ai_type = NPC_AI_PLAYER_JUMP;
	npc_data[NPC_BAUUL].ai_max_z_diff = 99999;
	npc_data[NPC_BAUUL].anim_speed = 0.5f;
	
	// NPC_HARPY
	npc_data[NPC_HARPY].name = "Harpy";
	npc_data[NPC_HARPY].res_mesh = ResourceLoader::meshType::Harpy;
	npc_data[NPC_HARPY].max_health = 15;
	npc_data[NPC_HARPY].move_speed = 20;
	npc_data[NPC_HARPY].attack_type = NPC_ATTACK_TYPE_MELEE;
	npc_data[NPC_HARPY].attack_speed = 5;
	npc_data[NPC_HARPY].attack_range = 1200;
	npc_data[NPC_HARPY].damage = 10;
	npc_data[NPC_HARPY].bounty = 90;
	npc_data[NPC_HARPY].ai_type = NPC_AI_PLAYER_FLYING;
	npc_data[NPC_HARPY].ai_max_z_diff = 99999;
	npc_data[NPC_HARPY].anim_speed = 0.5;
	//npc_data[NPC_HARPY].snd_taunt1 = snd_create("harpy1.wav");
	//npc_data[NPC_HARPY].snd_taunt2 = snd_create("harpy2.wav");
}

s_npcs *b_npcs::at(uint id)
{
	if (id > B_NPCS_ENTRIES) {err(" b_npcs::at was called with invalid id", -1); return NULL;}
	return &npc_data[id];
}

void b_npcs::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_npcs in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
