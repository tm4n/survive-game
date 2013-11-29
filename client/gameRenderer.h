

#include "SDL.h"
#include "SDL_opengl.h"


class gameRenderer
{
public:
	gameRenderer();
	~gameRenderer();

	void drawFrame();
private:
	SDL_Window *window;
	SDL_GLContext glcontext;
};

