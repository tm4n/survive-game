#include "SDL2/SDL.h"
#include "SDL2/SDL_timer.h"
#undef main
#include "gameRenderer.h"
#include "GUI.h"
#include "Menu.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main(int argc, char **argv)
{

	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) exit(1);

	bool quit = false;

	gameRenderer *renderer = new gameRenderer();

	//display stuff
	RenderObject *objTerrain = new RenderObject();
	objTerrain->scale = glm::vec3(2.0f, 2.0f, 2.0f);
    renderer->resources.getMesh(ResourceLoader::meshType::Terrain)->addRenderObject(objTerrain);
        
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 50; j++) {
        RenderObject *tmp = new RenderObject();
        	
        tmp->translation[1] += i*50.f;
        tmp->translation[2] += j*50.f;
        	
        renderer->resources.getMesh(ResourceLoader::meshType::Crate)->addRenderObject(tmp);
        }
    }
        
    
	RenderObject *soldiers = new RenderObject[30];
    for (int i = 0; i < 30; i++) {       	
        soldiers[i].rotation[0] += i*180.f;
        	
        soldiers[i].translation[1] += i*80.f;
        soldiers[i].translation[0] += -200;
        	
        renderer->resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(&soldiers[i]);
    }

	// create trees at their location
	RenderObject *tmp = new RenderObject();
	tmp->translation = glm::vec3(-328.0f, -1400.0f, -312.0f);
	tmp->rotation[0] = 200.0f;
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
	tmp = new RenderObject();
	tmp->translation = glm::vec3(800.0f, 1296.0f, -300.0f);
	tmp->rotation[0] = 36.0f;
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);
	tmp = new RenderObject();
	tmp->rotation[0] = 6.0f;
	tmp->translation = glm::vec3(-1440.0f, -784.0f, -300.0f);
	renderer->resources.getMesh(ResourceLoader::meshType::Tree)->addRenderObject(tmp);

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
					renderer->gui->event_mouse(&evt);
				else
				{
					// TODO:
				}

				break;
			}
		}
		
		// animation test
		for (int i = 0; i < 30; i++) {
				
			if (soldiers[i].animProgress > 1.0f) {
				soldiers[i].animProgress -= 1.0f;
				soldiers[i].animFrame = soldiers[i].animNextFrame;
				soldiers[i].animNextFrame += 1;
				soldiers[i].animNextFrame %= 65;
			}
			soldiers[i].animProgress += 0.1f;
				
		}
		

		renderer->drawFrame();

		if (fct % 100 == 0) printf("FPS: %f\n", 1000.f / (SDL_GetTicks() - t));
		fct++;
	}

	delete renderer;

	SDL_Quit();

	return 0;
}
