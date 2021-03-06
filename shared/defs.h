#ifndef __DEFS_H__
#define __DEFS_H__

#define GLM_FORCE_PURE

#include "vec.h"
#include "stdio.h"

// General definitions, types

#ifdef _WIN32
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#endif

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#ifndef ANDROID
#define DEF_NAME_STR "Survive! 0.73 Alpha"
#else
#define DEF_NAME_STR "Survive! for Android 0.73 Alpha"
#endif // ANDROID

#define DEF_VERSION 110   // Version of network protocol
#define PLAYERNAME_LENGTH 32

#define RESPAWN_TIME 5.f;
#define MAX_PLAYERS 8
#define TICK_UPDATE_LIMIT 16  // update interval for regular chances, in ticks
#define CL_SEND_ANGLE_RATE 0.5f
#define SV_SEND_PLAYER_RATE 8.f
#define SV_SEND_NPC_RATE 8.f // 32.f for internet server recommended

#define CAMERA_VIEW_HEIGHT 1.f
#define CAMERA_BOB_STOP_RATE 1.5f


////////////////////////////////////////////
// functions used everywhere

/////////////////////////////////////////
// log messages, see main.cpp
#define LOG_DEBUG_VERBOSE 1
#define LOG_DEBUG 2
#define LOG_INFO 3
#define LOG_WARNING 4
#define LOG_ERROR 5

void log(int importance, const char *msg);


#endif // __DEFS_H__
