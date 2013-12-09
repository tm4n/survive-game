#include "gameRenderer.h"

#include <stdlib.h> 
#include "glm/gtc/matrix_transform.hpp"

gameRenderer::gameRenderer()
{
	// not sure if this is needed!
	// request OGL 3.2 context (default to SDL core profile)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // turn on double buffering with a 24 bit Z buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("Survive!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	glcontext = SDL_GL_CreateContext(window);

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		exit(-3);
	}

	// Initialize OpenGL features
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
       
    glClearDepth(1.0f);                     //Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);            //Enables Depth Testing
    glDepthFunc(GL_LEQUAL);             //The Type Of Depth Testing To Do
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float ratio = 800.f/600.f;
	mProjMatrix = glm::perspective(80.f, ratio, 2.f, 4000.f);

	// load all resources
	resources.load();


	//display stuff
	RenderObject *objTerrain = new RenderObject();
    resources.getMesh(ResourceLoader::meshType::Terrain)->addRenderObject(objTerrain);
        
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 50; j++) {
        RenderObject *tmp = new RenderObject();
        	
        tmp->translation[1] += i*50.f;
        tmp->translation[2] += j*50.f;
        	
        resources.getMesh(ResourceLoader::meshType::Crate)->addRenderObject(tmp);
        }
    }
        
    soldiers = new RenderObject[30];
    for (int i = 0; i < 30; i++) {       	
        soldiers[i].rotation[0] += i*180.f;
        	
        soldiers[i].translation[1] += i*80.f;
        soldiers[i].translation[0] += -200;
        	
        resources.getMesh(ResourceLoader::meshType::Soldier)->addRenderObject(&soldiers[i]);
    }
}

gameRenderer::~gameRenderer()
{
	SDL_GL_DeleteContext(glcontext);

	SDL_DestroyWindow(window);
}

void gameRenderer::drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// calculate matrices
	mVMatrix       = glm::lookAt(
    glm::vec3(-600,0,3), // Camera is at (4,3,3), in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,0,1)  // Head is up (set to 0,-1,0 to look upside-down)
    );

	mVPMatrix = mProjMatrix * mVMatrix;

	// Animation test
    for (int i = 0; i < 30; i++) {
        	
        if (soldiers[i].animProgress > 1.0f) {
        	soldiers[i].animProgress -= 1.0f;
        	soldiers[i].animFrame = soldiers[i].animNextFrame;
        	soldiers[i].animNextFrame += 1;
        	soldiers[i].animNextFrame %= 65;
        }
        soldiers[i].animProgress += 0.1f;
        	
    }

	// draw all meshes
    for (int i = 0; i < MAX_MESHES; i ++) {
		Mesh *m = resources.getMesh(static_cast<ResourceLoader::meshType>(i));
        if (m != NULL) m->draw(mVPMatrix);
    }


	SDL_GL_SwapWindow(window);
}