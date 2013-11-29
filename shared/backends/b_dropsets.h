#ifndef __B_DROPSETS_H__
#define __B_DROPSETS_H__

#include "defs.h"
#include <iostream>
#include <string>
#include <list>
#include "item_cont.h"

#define B_DROPSETS_ENTRIES 256

struct s_drop{
	ushort item_id;
	float chance;
};

struct s_dropset{

    std::list<s_drop> drops;
};


class b_dropsets
{
public:
	s_dropset dropsets[B_DROPSETS_ENTRIES];

	b_dropsets(const char *);

	s_dropset *at(uint id);

	bool fill_loot(item_cont *ct, uint dropset_id);

private:
	void err(const char *, int);
};


// globally available instance
extern b_dropsets *b_drp;

#endif
