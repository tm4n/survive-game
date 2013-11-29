#ifndef __B_SKILLS_H__
#define __B_SKILLS_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include "bufflist.h"

#define B_SKILLS_ENTRIES 200

// target types
#define SK_TARGET_TYPE_NONE 0
#define SK_TARGET_TYPE_SELF 1
#define SK_TARGET_TYPE_ACTOR_FRIENDLY 2
#define SK_TARGET_TYPE_ACTOR_ENEMY 3
#define SK_TARGET_TYPE_ACTOR_ANY 4
#define SK_TARGET_TYPE_GROUND 5

// skill requirement types (only applicable for players)
#define SK_REQUIREMENT_LEVEL 2
#define SK_REQUIREMENT_STRE 3
#define SK_REQUIREMENT_AGIL 4
#define SK_REQUIREMENT_INTEL 5
#define SK_REQUIREMENT_SKILL 6

// skill action types
#define SK_ACTION_TYPE_INSTANT 1
#define SK_ACTION_TYPE_WAVE 2
#define SK_ACTION_TYPE_PROJECTILE 3

// skill action hits

//1-100 reserved for BUFF.. from bufflist.h

#define SK_ACTION_CHANGE_HEALTH 101
#define SK_ACTION_CHANGE_MANA 102

// skill action affected actors
#define SK_ACTION_AFFECTED_ALL 1
#define SK_ACTION_AFFECTED_ENEMY 2
#define SK_ACTION_AFFECTED_FRIENDLY 3
#define SK_ACTION_AFFECTED_SELF 4

struct s_sreq{
    int type;
    int val;
};

struct s_action{
	int type;
	int hit_type;
	int affected;
	float splash_range;
	float speed;
	float val1_start;
	float val1_add;
	float val2_start;
	float val2_add;
	std::string file;
	int effect_start;
	int effect_projectile;
	int effect_hit;
};

struct s_skill{
	bool valid;
	int bclass_id;
	int bskilltree_cat;
	int bskilltree_num;
	std::string bname, bdesc;
	void *icon;
	float bmanacost_start;
	float bmanacost_add;
	float bcooldown_start;
	float bcooldown_add;
	int btarget_type;
	int brange;

    std::list<s_sreq> requirements;
	std::vector<s_action> actions;
};

class b_skills
{
public:
	s_skill skills[B_SKILLS_ENTRIES];

	b_skills(const char *);

	s_skill *at(uint id);

	float get_cooldown(uint id, ushort level);
	float get_manacost(uint id, ushort level);

	std::string get_skill_desc(uint id, ushort level);
	std::string get_skill_info(uint id, ushort level);

private:
	void err(const char *, int);
};


// globally available instance
extern b_skills *b_sk;

#endif
