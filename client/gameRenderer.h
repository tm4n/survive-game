#ifndef __GAMERENDERER_H__
#define __GAMERENDERER_H__

#include "SDL2/SDL.h"

#ifdef ANDROID
#include "SDL_opengles2.h"
#include <GLES2/gl2.h>
#else
#include "gl_core_2_1.h"
#include "SDL2/SDL_opengl.h"
#endif

#include "ResourceLoader.h"
#include "GUI.h"
#include "particlemgr.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"


class gameRenderer
{
public:
	ResourceLoader resources;
	GUI *gui;
	particlemgr *partmgr;

	gameRenderer();
	~gameRenderer();

	void drawFrame(double time_delta);

	glm::vec3 CameraPos;
	glm::vec3 CameraAngle;
	
	float CameraJoyInputY;
	float CameraJoyInputX;
private:
	SDL_Window *window;
	SDL_GLContext glcontext;

	glm::mat4 mVPMatrix;
	glm::mat4 mProjMatrix;
	glm::mat4 mVMatrix;
};


#endif
