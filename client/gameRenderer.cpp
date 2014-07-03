#include "gameRenderer.h"

#include "helper.h"
#include <iostream>
#include <stdlib.h> 
#include "glm/gtc/matrix_transform.hpp"

gameRenderer::gameRenderer()
{
	int ss_x = 1366, ss_y = 768;

	// not sure if this is needed!
	// request OGL 2.1 context (default to SDL core profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // turn on double buffering with a 24 bit Z buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

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

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		exit(-3);
	}

	// Initialize OpenGL features
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
       
    glClearDepth(1.0f);                     //Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);            //Enables Depth Testing
    glDepthFunc(GL_LEQUAL);             //The Type Of Depth Testing To Do
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float ratio = 16.f/9.f;
	mProjMatrix = glm::perspective(70.f, ratio, 2.f, 6000.f);

	// load all resources
	resources.load();

	// initialize gzu
	gui = new GUI();
	gui->setScreensize(ss_x, ss_y);
}

gameRenderer::~gameRenderer()
{
	if (gui) delete gui;

	//SDL_GL_DeleteContext(glcontext); TODO: find out if needed

	SDL_DestroyWindow(window);
}

void gameRenderer::drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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


	// draw all meshes
    for (int i = 0; i < MAX_MESHES; i ++) {
		Mesh *m = resources.getMesh(static_cast<ResourceLoader::meshType>(i));
        if (m != NULL) m->draw(mVPMatrix);
    }

	// draw gui over it
	gui->draw();


	SDL_GL_SwapWindow(window);
}
