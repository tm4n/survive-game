#ifndef __SOUND_H__
#define __SOUND_H__

#include "SDL/SDL_mixer.h"
#include <string>
#include "actor.h"

class Sound {

public:
	bool loaded;

	std::string file;

	Sound(const char* filename);
	~Sound();

	void play(int num, float volume);
	void play3D(int num, actor *ac, float volume);

private:
	Mix_Chunk *chunk;
};

#endif //__SOUND_H__
