#ifndef __B_EFFECTS_H__
#define __B_EFFECTS_H__

#include <windows.h>
#include "var.h" // the var class - without it, you'd need _VAR() macros
#include "adll.h"
#include <string>
#include <list>
#include <vector>
#include "defs.h"
#include "helper.h"
#include "vec.h"
#include "b_gui.h"

#define B_EFFECTS_ENTRIES 300

#define EFFECT_TYPE_SOUND 1
#define EFFECT_TYPE_DELAY 2
#define EFFECT_TYPE_MESSAGE 3
#define EFFECT_TYPE_PARTICLES 4
#define EFFECT_TYPE_SPRITE 5
#define EFFECT_TYPE_REPEAT 6
#define EFFECT_TYPE_MESSAGE_3D 7


#define PAR_FNC_TYPE_VEL_RANDOM 1
#define PAR_FNC_TYPE_VERTEX_RANDOM 2
#define PAR_FNC_TYPE_SIZE_RANDOM 3


#define PAR_EVENT_TYPE_ALPHAFADE 1

#define SPRITE_POS_TYPE_START 1
#define SPRITE_POS_TYPE_END 2
#define SPRITE_POS_TYPE_BETWEEN 3
#define SPRITE_POS_TYPE_LINE 4

struct s_instr {
	int type;
};

struct s_instr_message_3d : s_instr {
	std::string msg;
	int fontsize;
	uchar color[3];
	float duration; // in milliseconds
	float flash;
	float move_z;
	float offset_z;
};

struct s_instr_sprite : s_instr {
	std::string file;
	bool billboard;
	uchar color[3];
	float scale_start, scale_change;
	float fade_in_fac, fade_out_fac;
	float fade_in_start, fade_out_end, fade_max;
	float duration;
	float lightrange;
	int pos_type;
	long flags;
};

struct s_instr_particles : s_instr {
	int number;
	BMAP* bmap;
	float alpha;
	uchar color[3];
	float size;
	float lifespan;
	float gravity;
	float vel_x, vel_y, vel_z;
	int fnc_type;
	int event_type;
	float skill_a, skill_b, skill_c, skill_d;
	float skill_x, skill_y, skill_z;
	long flags;
};

struct s_instr_message : s_instr {
	std::string msg;
	uchar color[3];
	float duration; // in milliseconds
	float flash;
};

struct s_instr_delay : s_instr {
	float duration; // in milliseconds
};

struct s_instr_repeat : s_instr {
	int go_to;
};

struct s_instr_sound : s_instr {
	SOUND* snd;
	int volume;
	int balance;
	int frequency;
	int snd3d;
};

struct s_effect{
	std::string name;
	std::vector<s_instr*> commands;
	bool valid;
};


struct s_effsched{
	float wait_time;
	uint id;
	uint instr;
	vec pos_start;
	vec pos_end;
	ENTITY *caster;
	ENTITY *target;
	int i1, i2;
	float f1, f2;
	std::string s1, s2, s3;
};

struct s_message_3d_sched{
	TEXT *txt;
	float duration;
	s_instr_message_3d *msg3d_data;
	VECTOR pos;
};

class b_effects
{
public:
	s_effect effects[B_EFFECTS_ENTRIES];

	b_effects(b_gui* gui, const char *);

	s_effect *at(uint id);

	std::list<ENTITY*> active_sprites;
	std::list<s_message_3d_sched> active_msg3d;

	void frame(double time_delta);
	void handle_sprite(ENTITY* sprite, double time_delta);
	void handle_msg3d(s_message_3d_sched *s3, double time_delta);

	void disable_all();

	void play(uint id);
	void play(uint id, vec *pos);
	void play(uint id, vec *pos_start, vec *pos_end);
	void play(uint id, ENTITY *cast, vec *pos_start, ENTITY* target, vec *pos_end);
	void play(uint id, ENTITY *ent, vec *start);
	void play(uint id, ENTITY* ent);
	void play(uint id, ENTITY* cast, ENTITY *target);
	void play(uint id, ENTITY* ent, int i1, int i2);
	void play(uint id, int i1, int i2, float f1, float f2, std::string *s1, std::string *s2, std::string *s3);

	void stop(uint id, ENTITY* ent);
	void stop_ent(ENTITY* ent);

private:
	void err(const char *, int);

	FONT* font;

	double msg_alpha[3];
	double msg_duration[3];
	b_gui *gui;
	uint txt_message_id[3];
	uint txt_message_group[3];
	bool txt_message_use[3];

	std::list<s_effsched> schedule;
};


// globally available instance
extern b_effects *b_ef;


#define EFF_BUFFS_BASE 100

// fixed ids
#define EFF_GENERIC_BUTTON_CLICK 1
#define EFF_GENERIC_BUTTON_CLOSE 2
#define EFF_GOLD_RECEIVED 3
#define EFF_GOLD_REMOVED 4

#define EFF_IT_MOVE_SUCCESS 5 // for all moves, including buy and sell
#define EFF_IT_MOVE_FAIL 6
#define EFF_IT_SELL_FAIL 7
#define EFF_IT_BUY_FAIL 8
#define EFF_IT_ADD 9
#define EFF_IT_REMOVE 10
#define EFF_IT_INV_FULL 11

#define EFF_LOOT 15
#define EFF_SHOP 16

#define EFF_SKILL_INCREASE 17

#define EFF_LEVEL_UP 18
#define EFF_XP_RECEIVED 19

#define EFF_DIALOGUE_OPEN 20
#define EFF_DIALOGUE_ANSWER_HIGHLIGHT 21
#define EFF_DIALOGUE_ANSWER_CLICK 22

#define EFF_SKILL_NO_TARGET_ACTOR 25
#define EFF_SKILL_OUT_OF_RANGE 26
#define EFF_SKILL_NO_MANA 27
#define EFF_SKILL_COOLDOWN 28
#define EFF_SKILL_INVALID_TARGET 29

#define EFF_QUEST_OPEN 30
#define EFF_QUEST_PROG_ITEM 31
#define EFF_QUEST_PROG_KILL 32
#define EFF_QUEST_PROG_TALK 33
#define EFF_QUEST_PROG_BRING 34
#define EFF_QUEST_FINISH 35

#define EFF_GATEWAY 36

#define EFF_LOOTABLE 40
#define EFF_MOVE_ORDER 45


#endif
