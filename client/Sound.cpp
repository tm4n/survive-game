#include "Sound.h"

#include <iostream>
#include <string>

Sound::Sound(const char* filename)
{
	chunk = NULL;
	loaded = false;

	file.assign(filename);

	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if (file == NULL) { std::cout << "ERROR on SDL_RWFromFile while opening sound file: " << filename << std::endl;  return; }

	chunk = Mix_LoadWAV_RW(file, 1);
	if (chunk == NULL) { std::cout << "ERROR in Mix_LoadWAV_RW wh while opening sound file: " << filename << std::endl;  return; }

	loaded = true;
}

void Sound::play()
{
	if (Mix_PlayChannel(-1, chunk, 0) == -1)
	{
		{ std::cout << "ERROR in Mix_PlayChannel: " << Mix_GetError() << std::endl;  return; }
	}
}