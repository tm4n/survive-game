
#include "gl_opengl_3_2.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "ResourceLoader.h"
#include "glm/glm.hpp"


class gameRenderer
{
public:
	ResourceLoader resources;

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

	//DEBHUG
	RenderObject *soldiers;
};

