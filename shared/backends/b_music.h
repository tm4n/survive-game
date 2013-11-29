#ifndef __B_MUSIC_H__
#define __B_MUSIC_H__

#include <windows.h>
#include "var.h" // the var class - without it, you'd need _VAR() macros
#include "adll.h"
#include "defs.h"
#include <iostream>
#include <string>
#include <vector>

#define B_MUSIC_ENTRIES 50

#define LOGIN_MUSIC_SCORE 1

struct s_sndptrs{
	SOUND *snd_r;
	SOUND *snd_l;
};


struct s_track{
	std::string file_l, file_r;
    int volume;
	float delay_after;
	int danger_level; 
};

struct s_score{
	std::string name;
	std::vector<s_track> tracks;
	bool valid;
};


class b_music
{
public:
	b_music(const char *);

	void play(uint score_id);
	void stop();

	void frame(double time_step, int danger_level);
private:
	void err(const char *, uint);
	int get_next_song(uint score_id, int danger_level);

	s_score scores[B_MUSIC_ENTRIES];

	double wait_timer;

	int next_score;
	int current_score;
	int current_track;
	double current_volume;
	double fade_fac;

	int current_danger_level;

	long snd_handle_l;
	long snd_handle_r;

	SOUND *current_l, *current_r;

	std::vector<s_sndptrs> sndbuffer;
};


// globally available instance
extern b_music *b_mu;

#endif
