#include "SDL2/SDL.h"
#include "SDL2/SDL_timer.h"
#undef main
#include "gameClient.h"
#include "gameRenderer.h"
#include "GUI.h"
#include "Menu.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char **argv)
{
	if (enet_initialize() != 0) exit(1);

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) exit(1);

	bool quit = false;

	gameRenderer *renderer = new gameRenderer();

	gameClient *cl = NULL;

	// create menu
	Menu *menu = new Menu(renderer->gui, &renderer->resources);

	int fct = 0;
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

			case SDL_MOUSEBUTTONDOWN:

				if (menu->visible)
				{
					menu->hide();
					renderer->gui->event_mouse(&evt);

					cl = new gameClient(renderer);
					cl->connect("192.168.0.20", 1201);
				}
				else
				{
					// TODO:
				}

				break;
			}
		}
		
		// animation test
		/*for (int i = 0; i < 30; i++) {
				
			if (soldiers[i].animProgress > 1.0f) {
				soldiers[i].animProgress -= 1.0f;
				soldiers[i].animFrame = soldiers[i].animNextFrame;
				soldiers[i].animNextFrame += 1;
				soldiers[i].animNextFrame %= 65;
			}
			soldiers[i].animProgress += 0.1f;
				
		}*/
		
		if (cl) cl->frame(0.0);

		renderer->drawFrame();

		if (fct % 100 == 0) printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
		fct++;
	}

	delete renderer;

	SDL_Quit();

	return 0;
}
