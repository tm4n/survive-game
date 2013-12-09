#include "SDL.h"
#include "SDL_timer.h"
#undef main
#include "gameRenderer.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char **argv)
{

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) exit(1);

	bool quit = false;

	gameRenderer *renderer = new gameRenderer();



	while (!quit)
	{
		Uint32 t = SDL_GetTicks();
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

		printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
	}

	delete renderer;

	SDL_Quit();

	return 0;
}
