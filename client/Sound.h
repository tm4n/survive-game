#ifndef __SOUND_H__
#define __SOUND_H__

#include "SDL2/SDL_mixer.h"
#include <string>

class Sound {

public:
	bool loaded;

	std::string file;

	Sound(const char* filename);
	~Sound();

	void play();

private:
	Mix_Chunk *chunk;
};

#endif __SOUND_H__