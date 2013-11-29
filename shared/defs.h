#ifndef __DEFS_H__
#define __DEFS_H__

#include "vec.h"
#include "stdio.h"

// General definitions, types

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#define DEF_VERSION 110

#define PRJ_RESPAWN_TIME 10

#define UPDATE_RANGE 600 // update range in quants
#define VIS_RANGE 700 // visibility range

#define TICK_UPDATE_LIMIT 16  // update interval for regular chances, in ticks
#define PRJ_INV_EQUIPMENT 8
#define PRJ_INV_SIZE  48  // 20 + PRJ_INV_EQUIPMENT
#define PRJ_LOOT_SIZE 8
#define PRJ_SHOP_SIZE 16
#define PRJ_SKILLS_SIZE 32
#define PRJ_SKILLBAR_SIZE 10

#define PRJ_GUI_INV_SPACING 52
#define PRJ_GUI_QUESTLOG_SPACING 70
#define PRJ_GUI_DIALOGUE_ANSWERS_SPACING 40
#define PRJ_GUI_SKILLS_SPACING 70
#define PRJ_GUI_SKILLBAR_SPACING 51

#define PRJ_QUESTLOG_SIZE 64

#define PRJ_INTERACT_RANGE 90


////////////////////////////////////////////
// functions used everywhere

/////////////////////////////////////////
// log messages, see main.cpp
#define LOG_DEBUG_VERBOSE 1
#define LOG_DEBUG 2
#define LOG_WARNING 3
#define LOG_ERROR 4

void log(int importance, const char *msg);


#endif // __DEFS_H__
