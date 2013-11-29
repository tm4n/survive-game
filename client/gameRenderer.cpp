#include "gameRenderer.h"

#include <stdlib.h>  

gameRenderer::gameRenderer()
{
	window = SDL_CreateWindow("Survive!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	glcontext = SDL_GL_CreateContext(window);
}

gameRenderer::~gameRenderer()
{
	SDL_GL_DeleteContext(glcontext);

	SDL_DestroyWindow(window);
}

void gameRenderer::drawFrame()
{
	glClearColor((rand() % 256) / 256.0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
}