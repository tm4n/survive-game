#include "SDL.h"
#include "gameRenderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) exit(1);

	bool quit = false;

	gameRenderer *renderer = new gameRenderer();

	while (!quit)
	{
		SDL_Event evt;
		while( SDL_PollEvent(&evt) )
		{
			switch(evt.type)
			{
			case SDL_QUIT:  quit = true;   break;
			/* process other events you want to handle here */
			}
		}

		renderer->drawFrame();
	}

	delete renderer;

	SDL_Quit();

	return 0;
}
