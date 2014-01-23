#include "SDL2/SDL.h"
#include "SDL2/SDL_timer.h"
#undef main
#include "gameClient.h"
#include "../server/gameServer.h"
#include "gameRenderer.h"
#include "GUI.h"
#include "Menu.h"
#include "net.h"
#include <thread>


gameClient *cl = NULL;
std::thread svthread;
gameServer *sv = NULL;
gameRenderer *renderer = NULL;
Menu *menu = NULL;

std::mutex m1, m2;
std::list<ENetPacket*> l1, l2;

// Callback classes
class playCallback : public GUICallback {

	public:
		virtual void callback(int obj_id);

};

void thread_sv()
{
	sv = new gameServer(&l1, &m1, &l2, &m2);
	sv->run();
	delete sv;
	sv = NULL;
}


void playCallback::callback(int obj_id)
{
	menu->hide();
	
	// start server thread
	svthread = std::thread(thread_sv);
	
	// start client in this thread
	cl = new gameClient(renderer);
	cl->connect(&l2, &m2, &l1, &m1);
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char **argv)
{
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
		
		if (cl) cl->frame(0.1);

		renderer->drawFrame();

		//if (fct % 100 == 0) printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
		fct++;
	}

	if (sv != NULL)
	{
		sv->quit = true;
		svthread.join();
		delete sv;
	}

	delete renderer;

	SDL_Quit();
	
	if (cl) delete cl;

	return 0;
}
