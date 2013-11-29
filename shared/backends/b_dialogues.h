#ifndef __B_DIALOGUES_H__
#define __B_DIALOGUES_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <list>
#include <vector>

#define B_DIALOGUES_ENTRIES 256

#define B_DIALOGUES_MAX_ANSWERS 5


// definitions of dialogue answer actions
#define DI_ANSWER_QUEST_ACCEPT 1
#define DI_ANSWER_NEXT_PART 2
#define DI_ANSWER_CLOSE 3
#define DI_ANSWER_SHOP 4
#define DI_ANSWER_QUEST_BRING 5

struct s_part_answer{
	std::string text;
    int action;
    int val;
};

struct s_part{
	int id;
	std::string text;
	std::vector<s_part_answer> answers;
};

struct s_dialogue{
	std::list<s_part> parts;
};

class b_dialogues
{
public:
	s_dialogue dialogues[B_DIALOGUES_ENTRIES];

	b_dialogues(const char *);

	s_dialogue *at(uint id);

	std::string str_err;

	std::string get_text(uint dialogue_id, int part_id, std::string* plname);
	s_part_answer *get_answer(uint dialogue_id, uint part_id, int answer_id);

private:
	void err(const char *, int);
};


// globally available instance
extern b_dialogues *b_di;

#endif
