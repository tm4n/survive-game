#include "gameRenderer.h"

#include "helper.h"
#include <iostream>
#include <stdlib.h> 
#include "glm/gtc/matrix_transform.hpp"
#include "SDL/SDL_mixer.h"

gameRenderer::gameRenderer()
{
	CameraJoyInputY = 0.f;
	CameraJoyInputX = 0.f;

	// Initialize sound
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1 )
	{
		std::cout << "SDL Error on open SDL Mixer Audio: " << Mix_GetError() << std::endl;
		exit(-3);
	}
	
	#ifdef ANDROID
	int ss_x = 1920, ss_y = 1080;
	#else
	int ss_x = 1366, ss_y = 768;
	#endif

	// not sure if this is needed!
	#ifndef ANDROID
	// request OGL 2.1 context (default to SDL core profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // turn on double buffering with a 24 bit Z buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	#endif

	window = SDL_CreateWindow("Survive!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ss_x, ss_y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cout << "SDL Error on window opening: " << SDL_GetError() << std::endl;
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

	float ratio = 16.f/9.f;
	mProjMatrix = glm::perspective(70.f, ratio, 2.f, 6000.f);

	// load menu resources
	resources.loadMenu();
	
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

void gameRenderer::drawFrame(double time_delta)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// move camera by input (joy only, mouse done in event)
	if (CameraJoyInputY < 2600 && CameraJoyInputY > -2600) CameraJoyInputY = 0.f;
	if (CameraJoyInputX < 2600 && CameraJoyInputX > -2600) CameraJoyInputX = 0.f;
	CameraAngle.y -= CameraJoyInputY*0.0001f*(float)time_delta;
	CameraAngle.x -= CameraJoyInputX*0.0002f*(float)time_delta;

	// calculate matrices
	float camx = (float) (cos(toRadians(CameraAngle.x))*cos(toRadians(CameraAngle.y)));
    float camy = (float) (sin(toRadians(CameraAngle.x))*cos(toRadians(CameraAngle.y)));
    float camz = (float) (sin(toRadians(CameraAngle.y)));
	mVMatrix       = glm::lookAt(
    glm::vec3(CameraPos.x, CameraPos.y, CameraPos.z), // Camera in World Space
    glm::vec3(CameraPos.x + camx, CameraPos.y + camy, CameraPos.z + camz), // and looks t the eye point
    glm::vec3(0,0,1)  // Head is up (set to 0,-1,0 to look upside-down)
    );

	mVPMatrix = mProjMatrix * mVMatrix;

	// TODO: draw skybox

	// draw all meshes
    for (int i = 0; i < MAX_MESHES; i ++) {
		Mesh *m = resources.getMesh(static_cast<ResourceLoader::meshType>(i));
        if (m != NULL) m->draw(mVPMatrix);
    }

	// Draw particles
	partmgr->draw(time_delta, mVPMatrix, CameraPos);

	

	// draw gui over it
	gui->draw();


	SDL_GL_SwapWindow(window);
}
