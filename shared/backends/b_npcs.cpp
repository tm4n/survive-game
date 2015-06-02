#include "b_npcs.h"
#include "helper.h"
#include <sstream>

b_npcs::b_npcs()
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
	npc_data[NPC_MUMMY].bb_max.x = 17.588631f; npc_data[NPC_MUMMY].bb_max.y = 13.339565f; npc_data[NPC_MUMMY].bb_max.z = 36.198421f;
	npc_data[NPC_MUMMY].bb_min.x = -11.324703f; npc_data[NPC_MUMMY].bb_min.y = -15.327499f; npc_data[NPC_MUMMY].bb_min.z = -32.460072f;
	npc_data[NPC_MUMMY].res_snd_taunt1 = ResourceLoader::sndType::Zombie1;
	npc_data[NPC_MUMMY].res_snd_taunt2 = ResourceLoader::sndType::Zombie2;
	
	
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
	npc_data[NPC_WEREWOLF].bb_max.x = 24.940651f; npc_data[NPC_WEREWOLF].bb_max.y = 24.530399f; npc_data[NPC_WEREWOLF].bb_max.z = 34.555672f;
	npc_data[NPC_WEREWOLF].bb_min.x = -29.585518f; npc_data[NPC_WEREWOLF].bb_min.y = -27.091246f; npc_data[NPC_WEREWOLF].bb_min.z = -29.055637f;
	npc_data[NPC_WEREWOLF].res_snd_taunt1 = ResourceLoader::sndType::None;
	npc_data[NPC_WEREWOLF].res_snd_taunt2 = ResourceLoader::sndType::None;

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
	npc_data[NPC_SMASHER].bb_max.x = 30.127815f; npc_data[NPC_SMASHER].bb_max.y = 34.833366f; npc_data[NPC_SMASHER].bb_max.z = 30.635017f;
	npc_data[NPC_SMASHER].bb_min.x = -19.849461f; npc_data[NPC_SMASHER].bb_min.y = -38.439285f; npc_data[NPC_SMASHER].bb_min.z = -48.583107f;
	npc_data[NPC_SMASHER].res_snd_taunt1 = ResourceLoader::sndType::None;
	npc_data[NPC_SMASHER].res_snd_taunt2 = ResourceLoader::sndType::None;

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
	npc_data[NPC_WITCH].anim_speed = 1.f;
	//npc_data[NPC_WITCH].npc_attach = str_create("witch_stick.mdl");
	npc_data[NPC_WITCH].bb_max.x = 28.478836f; npc_data[NPC_WITCH].bb_max.y = 12.757271f; npc_data[NPC_WITCH].bb_max.z = 13.369316f;
	npc_data[NPC_WITCH].bb_min.x = -14.856464f; npc_data[NPC_WITCH].bb_min.y = -16.691643f; npc_data[NPC_WITCH].bb_min.z = -32.391720f;
	npc_data[NPC_WITCH].res_snd_taunt1 = ResourceLoader::sndType::None;
	npc_data[NPC_WITCH].res_snd_taunt2 = ResourceLoader::sndType::None;

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
	npc_data[NPC_HOGMAN].anim_speed = 1.f;
	npc_data[NPC_HOGMAN].bb_max.x = 44.935059f; npc_data[NPC_HOGMAN].bb_max.y = 45.910545f; npc_data[NPC_HOGMAN].bb_max.z = 34.989952f;
	npc_data[NPC_HOGMAN].bb_min.x = -27.529774f; npc_data[NPC_HOGMAN].bb_min.y = -49.378990f; npc_data[NPC_HOGMAN].bb_min.z = -58.357849f;
	npc_data[NPC_HOGMAN].res_snd_taunt1 = ResourceLoader::sndType::Monster;
	npc_data[NPC_HOGMAN].res_snd_taunt2 = ResourceLoader::sndType::None;

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
	npc_data[NPC_KNIGHT].anim_speed = 1.f;
	npc_data[NPC_KNIGHT].bb_max.x = 19.013863f; npc_data[NPC_KNIGHT].bb_max.y = 19.674980f; npc_data[NPC_KNIGHT].bb_max.z = 46.140266f;
	npc_data[NPC_KNIGHT].bb_min.x = -15.636204f; npc_data[NPC_KNIGHT].bb_min.y = -21.021851f; npc_data[NPC_KNIGHT].bb_min.z = -38.661064f;
	npc_data[NPC_KNIGHT].res_snd_taunt1 = ResourceLoader::sndType::None;
	npc_data[NPC_KNIGHT].res_snd_taunt2 = ResourceLoader::sndType::None;

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
	npc_data[NPC_BAUUL].bb_max.x = 33.458138f; npc_data[NPC_BAUUL].bb_max.y = 35.736652f; npc_data[NPC_BAUUL].bb_max.z = 48.873466f;
	npc_data[NPC_BAUUL].bb_min.x = -29.541290f; npc_data[NPC_BAUUL].bb_min.y = -41.254204f; npc_data[NPC_BAUUL].bb_min.z = -43.599411f;
	npc_data[NPC_BAUUL].res_snd_taunt1 = ResourceLoader::sndType::None;
	npc_data[NPC_BAUUL].res_snd_taunt2 = ResourceLoader::sndType::None;

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
	npc_data[NPC_HARPY].anim_speed = 0.5f;
	npc_data[NPC_HARPY].bb_max.x = 8.488274f; npc_data[NPC_HARPY].bb_max.y = 70.f; npc_data[NPC_HARPY].bb_max.z = 72.389915f;
	npc_data[NPC_HARPY].bb_min.x = -49.019192f; npc_data[NPC_HARPY].bb_min.y = -70.f; npc_data[NPC_HARPY].bb_min.z = 2.457153f;
	npc_data[NPC_HARPY].res_snd_taunt1 = ResourceLoader::sndType::Harpy1;
	npc_data[NPC_HARPY].res_snd_taunt2 = ResourceLoader::sndType::Harpy2;
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

	log(LOG_ERROR, pr.str().c_str());
}

// Thread safe singleton!
std::unique_ptr<b_npcs> b_npcs::m_instance;
std::once_flag b_npcs::m_onceFlag;

b_npcs * b_npcs::instance()
{
    std::call_once(m_onceFlag,
        [] {
            m_instance.reset(new b_npcs);
    });
    return m_instance.get();
}
