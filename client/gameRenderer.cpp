#include "gameRenderer.h"

#include "helper.h"
#include <iostream>
#include <stdlib.h> 
#include "glm/gtc/matrix_transform.hpp"
#include "SDL2/SDL_mixer.h"
#include "backends/b_settings.h"

gameRenderer::gameRenderer(int ss_x, int ss_y, float ratio, bool fullscreen, bool antialias)
{
	CameraJoyInputY = 0.f;
	CameraJoyInputX = 0.f;
	render_sky = false;

	// Initialize sound
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1 )
	{
		std::cout << "SDL Error on open SDL Mixer Audio: " << Mix_GetError() << std::endl;
		exit(-3);
	}
	Mix_AllocateChannels(SND_CHANNELS_ALLOC*2);
	Mix_GroupChannels(SND_CHANNELS_ALLOC, (SND_CHANNELS_ALLOC*2) - 1, SND_GROUP_2D);

	// not sure if this is needed!
	#ifndef ANDROID
	// request OGL 2.1 context (default to SDL core profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // turn on double buffering with a 24 bit Z buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	#endif

	if (antialias)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	window = SDL_CreateWindow("Survive!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ss_x, ss_y, flags);
	if (window == NULL)
	{
		std::cout << "SDL Error on window opening: " << SDL_GetError() << std::endl;
		std::cout << "Reverting to safe graphics settings, press a key to close this window, then restart the game." << std::endl;

		b_settings::instance()->set_secure_graphic_settings();

		std::cin.ignore();
		exit(-3);
	}

	glcontext = SDL_GL_CreateContext(window);
	if (glcontext == NULL)
	{
		std::cout << "SDL Error on creating OpenGL context: " << SDL_GetError() << std::endl;
		std::cout << "Your graphics card may not support OpenGL Version 2.1. You might want to try to update your graphics driver." << std::endl;
		std::cout << "NOT supported are old Intel GMA graphics cards." << std::endl;
		exit(-3);
	}

	#ifndef ANDROID
	// load openGL wrapped functions
	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		exit(-3);
	}

	if (antialias) glEnable(GL_MULTISAMPLE);  // not working in android
	#endif // ANDROID

	// Initialize OpenGL features
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
       
    glEnable(GL_DEPTH_TEST);            //Enables Depth Testing
    #ifdef ANDROID
	glClearDepthf(1.0f);                     //Depth Buffer Setup
	#else
	glClearDepth(1.0f);                     //Depth Buffer Setup
	#endif
    glDepthFunc(GL_LEQUAL);             //The Type Of Depth Testing To Do
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mProjMatrix = glm::perspective(70.f, ratio, 10.f, 8000.f);

	// load menu resources
	resources.loadMenu();
	// load skybox
	meshSkybox = new MeshSkybox((CubeTexture*)resources.getTex(ResourceLoader::texType::Sky));
	meshSkybox->initShader();
	
	// initialize gzu
	gui = new GUI();
	gui->setScreensize(ss_x, ss_y);

	// initialize particle manager
	partmgr = new particlemgr(&resources);
	
	log(LOG_DEBUG, "init done!");
}

gameRenderer::~gameRenderer()
{
	if (gui) delete gui;

	//SDL_GL_DeleteContext(glcontext); TODO: find out if needed

	SDL_DestroyWindow(window);

	Mix_CloseAudio();
}

// helper function for signum. can't believe cpp does not have this
double sgn(float val) {
    if (val < 0) return (-1.0); else return (1.0);
}

void gameRenderer::drawFrame(double time_delta)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// move camera by input (joy only, mouse done in event)
	if (CameraJoyInputY < 2600 && CameraJoyInputY > -2600) CameraJoyInputY = 0.f;
	if (CameraJoyInputX < 2600 && CameraJoyInputX > -2600) CameraJoyInputX = 0.f;
	CameraAngle.y -= (float)(sgn(CameraJoyInputY)*pow(CameraJoyInputY*0.00005, 2)*time_delta);
	double acc_x = pow(CameraJoyInputX*0.00006, 2)*time_delta*1.5;
	if (CameraJoyInputX > 31800 || CameraJoyInputX < -31800) acc_x *= 1.5;
	CameraAngle.x -= (float)(sgn(CameraJoyInputX)*acc_x);
	//CameraAngle.y -= CameraJoyInputY*0.0001f*(float)time_delta;
	//CameraAngle.x -= CameraJoyInputX*0.0002f*(float)time_delta;

	// calculate matrices
	float camx = (float) (cos(toRadians(CameraAngle.x))*cos(toRadians(CameraAngle.y)));
    float camy = (float) (sin(toRadians(CameraAngle.x))*cos(toRadians(CameraAngle.y)));
    float camz = (float) (sin(toRadians(CameraAngle.y)));
	glm::vec3 lookDir(CameraPos.x + camx, CameraPos.y + camy, CameraPos.z + camz);
	mVMatrix       = glm::lookAt(
    glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z), // Camera in World Space
    lookDir,
    glm::vec3(0,0,1)  // Head is up 
    );

	mVPMatrix = mProjMatrix * mVMatrix;


	// draw all meshes
    for (int i = 0; i < MAX_MESHES; i ++) {
		Mesh *m = resources.getMesh(static_cast<ResourceLoader::meshType>(i));
        if (m != NULL) m->draw(mVPMatrix);
    }
    // workaroud to always draw muzzle flash. seems not to be needed
    //Mesh *m = resources.getMesh(ResourceLoader::meshType::Muzzleflash);
    //if (m != NULL) m->draw(mVPMatrix);

	// draw skybox
	if (render_sky) meshSkybox->draw(mProjMatrix, mVMatrix);

	// TODO: draw transparent stuff sorted

	// Draw particles
	partmgr->draw(time_delta, mVPMatrix, CameraPos);

	

	// draw gui over it
	gui->draw();


	SDL_GL_SwapWindow(window);

	// emulate 3D sound
	for (int i = 0; i < MAX_MESHES; i++) {
		Mesh *m = resources.getMesh(static_cast<ResourceLoader::meshType>(i));
		if (m != NULL)
		{
			for (RenderObject *ro : m->objectList)
			{
				std::list<int>::iterator i = ro->attachedSndChannels.begin();
				while (i != ro->attachedSndChannels.end())
				{
					if (Mix_Playing(*i) == 0)
					{
						Sound::channels3D_free[*i] = true;
						ro->attachedSndChannels.erase(i++);  // alternatively, i = items.erase(i);
					}
					else
					{
						// update sound positions
						vec vro(ro->translation.x, ro->translation.y, ro->translation.z);
						vec vcam(CameraPos.x, CameraPos.y, CameraPos.z);

						float dist = vro.dist(&vcam) / 8;
						if (dist > 255.f) dist = 255.f;
						Uint8 idist = (Uint8)dist;

						if (vro.dist2d(&vcam) < 20.f)
						{
							// simplified effect without directional audio (still not perfect effect)
							Mix_SetPosition(*i, (Uint16)0, idist);
						}
						else
						{
							// full effect
							vec v(vro.x - vcam.x, vro.y - vcam.y, vro.z - vcam.z);
							float ang_to_obj = vec::angle(90.0f - vec::angle(atan2(v.x, v.y))*(float)(180.0 / M_PI));

							float ang = vec::angle(ang_to_obj - CameraAngle.x) + 180.f;

							Mix_SetPosition(*i, (Uint16)ang, idist);
						}
						++i;
					}
				}
			}
		}
	}
}
