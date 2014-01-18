#include "SDL2/SDL.h"
#include "SDL2/SDL_timer.h"
#undef main
#include "gameClient.h"
#include "gameRenderer.h"
#include "GUI.h"
#include "Menu.h"


gameClient *cl = NULL;
gameRenderer *renderer = NULL;
Menu *menu = NULL;

// Callback classes
class playCallback : public GUICallback {

	public:
		virtual void callback(int obj_id);

};


void playCallback::callback(int obj_id)
{
	menu->hide();
	
	cl = new gameClient(renderer);
	cl->connect("192.168.0.20", 1201);
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char **argv)
{
	if (enet_initialize() != 0) exit(1);

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) exit(1);

	bool quit = false;

	renderer = new gameRenderer();

	// create menu
	playCallback *pcb = new playCallback();
	menu = new Menu(renderer->gui, &renderer->resources, pcb);

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
			case SDL_MOUSEBUTTONUP:
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if (!menu->visible && cl != NULL)
				{
					cl->event_mouse(&evt);
				}
				else
				{
					renderer->gui->event_mouse(&evt);
				}
				break;

			case SDL_MOUSEMOTION:
				if (menu->visible == false && cl != NULL)
				{
					// give to gameClient
					cl->event_mouse(&evt);
				}
				break;
			}
		}
		
		if (cl) cl->frame(0.0);

		renderer->drawFrame();

		if (fct % 100 == 0) printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
		fct++;
	}

	delete renderer;

	SDL_Quit();

	return 0;
}
