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
#include "glm/glm.hpp"
#include "MeshSkybox.h"


class gameRenderer
{
public:
	ResourceLoader resources;
	GUI *gui;
	particlemgr *partmgr;

	MeshSkybox *meshSkybox;

	gameRenderer(int ss_x, int ss_y, float ratio, bool fullscreen, bool antialias);
	~gameRenderer();

	void drawFrame(double time_delta);

	int ss_x, ss_y;

	glm::vec3 CameraPos;
	glm::vec3 CameraAngle;

	glm::mat4 mVPMatrix;
	glm::mat4 mProjMatrix;
	glm::mat4 mVMatrix;

	bool render_sky;
	
	float CameraJoyInputY;
	float CameraJoyInputX;
private:
	SDL_Window *window;
	SDL_GLContext glcontext;
};


#endif
