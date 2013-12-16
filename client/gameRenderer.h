
#include "gl_core_2_1.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "ResourceLoader.h"
#include "GUI.h"
#include "glm/glm.hpp"


class gameRenderer
{
public:
	ResourceLoader resources;
	GUI *gui;

	gameRenderer();
	~gameRenderer();

	void drawFrame();

	glm::vec3 CameraPos;
	glm::vec3 CameraAngle;
private:
	SDL_Window *window;
	SDL_GLContext glcontext;

	glm::mat4 mVPMatrix;
	glm::mat4 mProjMatrix;
	glm::mat4 mVMatrix;
};

