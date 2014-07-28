﻿#include "SDL2/SDL.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_ttf.h"
#undef main
#include "gameClient.h"
#include "../server/gameServer.h"
#include "gameRenderer.h"
#include "GUI.h"
#include "Menu.h"
#include "net.h"
#include "Timer.h"
#include <thread>

bool quit = false;

gameClient *cl = NULL;
std::thread svthread;
gameServer *sv = NULL;
gameRenderer *renderer = NULL;
Menu *menu = NULL;

std::mutex m1, m2;
std::list<ENetPacket*> l1, l2;


void thread_sv()
{
	sv = new gameServer(&l1, &m1, &l2, &m2);
	sv->run();
	delete sv;
	sv = NULL;
}


// Callback classes
class playCallback : public GUICallback {

	public:
		virtual void callback(int obj_id);

};

void playCallback::callback(int obj_id)
{
	menu->hide();
	
	l2.clear();
	l1.clear();

	// start server thread
	svthread = std::thread(thread_sv);
	
	// start client in this thread
	cl = new gameClient(renderer);
	cl->connect(&l2, &m2, &l1, &m1);
}

// Callback classes
class quitCallback : public GUICallback {

	public:
		virtual void callback(int obj_id);

};

void quitCallback::callback(int obj_id)
{
	quit = true;
}


//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char **argv)
{
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) exit(1);
	if( TTF_Init()==-1 )
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}


	

	renderer = new gameRenderer();

	// create menu
	playCallback *pcb = new playCallback();
	quitCallback *qck = new quitCallback();
	menu = new Menu(renderer->gui, &renderer->resources, pcb, qck);

	// Timer used to calculate time_delta (frame time)
    Timer frametime;
	double time_delta = 0.;

	int fct = 0;
	while (!quit)
	{
		frametime.start();

		//Uint32 t = SDL_GetTicks();
		SDL_Event evt;
		while( SDL_PollEvent(&evt) )
		{
			switch(evt.type)
			{
			case SDL_QUIT:  quit = true;   break;
			/* process other events you want to handle here */

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if ((evt.key.keysym.sym == SDLK_ESCAPE && cl == NULL) || evt.key.keysym.sym == SDLK_F10)
				{
					quit = true;
					break;
				}
				// no break!!
				
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
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
			case SDL_MOUSEWHEEL:
				if (menu->visible == false && cl != NULL)
				{
					// give to gameClient
					cl->event_mouse(&evt);
				}
				break;
			}
		}
		
		if (cl) cl->frame(time_delta);

		renderer->drawFrame();

		//if (fct % 100 == 0) printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
		fct++;

		time_delta = ((double)frametime.get_ticks()) / (1000./16.);

		// check if client has quit
		if (cl) 
		{
			if (cl->quit == true)
			{
				delete cl; 
				cl = NULL;

				sv->quit = true;
				svthread.join();
				delete sv;
				menu->show();
			}
		}
	}

	if (sv != NULL)
	{
		sv->quit = true;
		svthread.join();
		delete sv;
	}

	delete renderer;

	TTF_Quit();
	SDL_Quit();
	
	if (cl) delete cl;

	return 0;
}
