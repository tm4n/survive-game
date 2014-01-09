#ifndef __B_ITEMS_H__
#define __B_ITEMS_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <list>

#define B_ITEMS_ENTRIES 256



// definitions of item categories
#define IT_CAT_OTHER 1
#define IT_CAT_WEAPON_RHAND 2
#define IT_CAT_RING 9
#define IT_CAT_POTION_HEALTH 20
#define IT_CAT_POTION_MANA 21

// definitions of item restrictions
#define IT_RESTR_CLASS 1
#define IT_RESTR_LEVEL 2
#define IT_RESTR_STRE 3
#define IT_RESTR_AGIL 4
#define IT_RESTR_INTEL 5

struct s_restr{
    uint type;
    int val;
};

struct s_item{
	std::string bfile, bname;

	void *icon; // to be used as BMAP* on client

	uint bcategory;
	int beffect;

	int bval_min[ITEM_NUM_VALUES]; //values, 2 for now
	int bval_max[ITEM_NUM_VALUES];

	int bgold_value;
	float bspeed;
	float brange;

	//TODO: specials

    std::list<s_restr> restrictions;
};

class b_items
{
public:
	s_item items[B_ITEMS_ENTRIES];

	b_items(const char *);

	s_item *at(ushort id);
	bool useable(ushort it_id);
	item *create_item(ushort it_id);
	void get_desc(std::string *, item *, int);

private:
	void err(const char *, int);
};


// globally available instance
extern b_items *b_it;

#endif
