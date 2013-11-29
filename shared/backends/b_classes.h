#ifndef __B_CLASSES_H__
#define __B_CLASSES_H__

#include "defs.h"
#include <iostream>
#include <string>

#define B_CLASSES_ENTRIES 20

struct s_class{
	std::string bfile, bname, binfo;

	// TODO:
	char bspell_cat[3][32];

	float bstart_health, badd_health;
	float bstart_stren, badd_stren;
	float bstart_agil, badd_agil;
	float bstart_intel, badd_intel;

	int battach_rhand, battach_rhand_align;
};

class b_classes
{
public:
	s_class classes[B_CLASSES_ENTRIES];

	b_classes(const char *);

	s_class *at(uint id);

private:
	void err(const char *, int);
};


// globally available instance

extern b_classes *b_cl;

#endif
