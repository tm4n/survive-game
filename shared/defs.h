#ifndef __DEFS_H__
#define __DEFS_H__

#include "vec.h"
#include "stdio.h"

// General definitions, types

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#define DEF_VERSION 110
#define PLAYERNAME_LENGTH 32

#define TICK_UPDATE_LIMIT 16  // update interval for regular chances, in ticks


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
