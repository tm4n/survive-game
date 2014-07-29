#ifndef __GAMERENDERER_H__
#define __GAMERENDERER_H__


#include "gl_core_2_1.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
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
private:
	SDL_Window *window;
	SDL_GLContext glcontext;

	glm::mat4 mVPMatrix;
	glm::mat4 mProjMatrix;
	glm::mat4 mVMatrix;
};


#endif
