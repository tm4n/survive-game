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
#include "helper.h"
#include "backends/b_settings.h"

const int FRAMES_PER_SECOND = 60;

bool quit = false;
bool play = false;
bool playMp = false;
bool multiplayer = false;

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
	menu->gui->draw();
	
	play = true;

	menu->snd_click();
}


// Callback classes
class mpCallback : public GUICallback {

public:
	virtual void callback(int obj_id);

};

void mpCallback::callback(int obj_id)
{
	menu->hide();
	menu->gui->draw();

	playMp = true;

	menu->snd_click();
}

// Callback classes
class quitCallback : public GUICallback {

	public:
		virtual void callback(int obj_id);

};

void quitCallback::callback(int obj_id)
{
	quit = true;

	menu->snd_click();
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#ifdef ANDROID
#include <android/log.h>

int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	// parse input arguments
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--debug") == 0) log_output_level = 0;
	}
	
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 ) exit(1);
	if( TTF_Init()==-1 )
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	// open renderer with correct settings
	b_settings *set = b_settings::instance();
	renderer = new gameRenderer(set->screenres_x, set->screenres_y, set->screenaspect, set->fullscreen, (set->antialias == 1));

	// create menu
	playCallback *pcb = new playCallback();
	mpCallback *mpcb = new mpCallback();
	quitCallback *qck = new quitCallback();
	menu = new Menu(renderer->gui, &renderer->resources, mpcb, pcb, qck);

	// Timer used to calculate time_delta (frame time)
    Timer frametime;
	double time_delta = 0.;
	Timer fps;
	
	// Initialize Joystick
	SDL_Joystick *joystick = NULL;
    SDL_JoystickEventState(SDL_ENABLE);


	std::cout << "Enter main loop" << std::endl;

	int fct = 0;
	while (!quit)
	{
		frametime.start();
		//Start the frame limit timer
        fps.start();

		if (joystick == NULL)
		{
			for (int i = 0; i < SDL_NumJoysticks(); i++)
			{
				SDL_Joystick *tmp_joystick = SDL_JoystickOpen(i);
				if (tmp_joystick && SDL_JoystickNumButtons(tmp_joystick) > 8) {joystick = tmp_joystick; i = 10000;}
					else SDL_JoystickClose(tmp_joystick);
			}
		}
		
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
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			case SDL_JOYAXISMOTION:
				if (!menu->visible && cl != NULL)
				{
					cl->event_input(&evt);
				}
				else
				{
					renderer->gui->event_mouse(&evt);
					// for special text editing keys
					menu->event_input_keys(&evt);
				}
				break;

			case SDL_MOUSEMOTION:
			case SDL_MOUSEWHEEL:
				if (menu->visible == false && cl != NULL)
				{
					// give to gameClient
					cl->event_input(&evt);
				}
				break;
				
			case SDL_JOYDEVICEREMOVED:
				joystick = NULL; // reset joystick on every removal, so a new one can be selected
				break;

			case SDL_TEXTINPUT:
			case SDL_TEXTEDITING:
				if (menu->visible)
				{
					menu->event_input_keys(&evt);
				}
				else if (cl != NULL)
				{
					cl->event_input(&evt);
				}
				break;
			}
		}
		
		if (menu) menu->frame(time_delta);
		if (cl) cl->frame(time_delta);

		renderer->drawFrame(time_delta);


		// check if client has quit
		if (cl) 
		{
			if (cl->quit == true)
			{
				delete cl; 
				cl = NULL;

				if (sv)
				{
					sv->quit = true;
					svthread.join();
					delete sv;
				}
				menu->show();
			}
		}
		else
		{
			if (play == true)
			{
				// play pressed!
				play = false;
				
				// load other resources
				menu->resources->loadIngame();
				
				l2.clear();
				l1.clear();

				// start server thread
				svthread = std::thread(thread_sv);
				
				// start client in this thread
				cl = new gameClient(renderer);
				cl->connect(&l2, &m2, &l1, &m1);
			}
			if (playMp == true)
			{
				// play pressed!
				playMp = false;
				const char *ip = menu->svlist->conn_ip.c_str();
				int port = menu->svlist->conn_port;


				// load other resources
				menu->resources->loadIngame();

				// start client in this thread
				cl = new gameClient(renderer);
				cl->connect(ip, port);
			}
		}

		// cap the frame rate
        if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }

		//if (fct % 100 == 0) printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
		fct++;

		time_delta = ((double)frametime.get_ticks()) / (1000./16.);
	}

	if (sv != NULL)
	{
		sv->quit = true;
		svthread.join();
		delete sv;
	}
	
	TTF_Quit();
	SDL_Quit();
	
	if (cl) delete cl;

	delete renderer;

	return 0;
}
