#ifndef __B_QUESTS_H__
#define __B_QUESTS_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <list>

#define B_QUESTS_ENTRIES PRJ_QUESTLOG_SIZE

#define B_QUESTS_GOAL_ENTRIES 3

// definitions of quest requirements
#define QE_REQUIREMENT_LEVEL 1
#define QE_REQUIREMENT_QUEST 2
#define QE_REQUIREMENT_CLASS 3
#define QE_REQUIREMENT_ITEM 4

// definitions of goal types, only 4 at a time
#define QE_GOAL_ITEM 1
#define QE_GOAL_KILL 2
#define QE_GOAL_TALK 3
#define QE_GOAL_BRING 4

#define QE_REWARD_ITEM 1
#define QE_REWARD_GOLD 2
#define QE_REWARD_XP 3
//#define QE_REWARD_SKILL 4

struct s_req{
    int type;
    int val;
};


struct s_goal{
	int type;
	int amount;  // may be used for other values, like item_id
	int target_id;
	int dialogue_id;
};

struct s_reward{
	int type;
	int id;
	int amount;
};

struct s_quest{
	std::string bname, bdesc, btext_open, btext_success;
	int bnpc_id, bdialogue_id, ballow_shop;

    std::list<s_req> requirements;
	s_goal goals[B_QUESTS_GOAL_ENTRIES];
	std::list<s_reward> rewards;
};

class b_quests
{
public:
	s_quest quests[B_QUESTS_ENTRIES];

	b_quests(const char *);

	s_quest *at(uint id);

private:
	void err(const char *, int);
};


// globally available instance
extern b_quests *b_qe;

#endif
