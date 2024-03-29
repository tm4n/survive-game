#include "CubeTexture.h"
#include <iostream>
#include <string>
#include "SDL2/SDL.h"
#include "helper.h"


CubeTexture::CubeTexture(std::vector<std::string> file) : Texture()
{
	if (file.size() != 6) return;
	files = file;

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

	tgaFiles = new tga_file*[6];
	for (int i = 0; i < 6; i++)
	{
		tgaFiles[i] = readTGA(file[i].c_str());
		if (tgaFiles[i] == NULL) { log(LOG_ERROR, "Error loading a tga file for a cube map");  return; }
	
		// now upload texture to OpenGL
		int colorMode = tgaFiles[i]->bitCount / 8;
		if (colorMode == 3) // BGR
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tgaFiles[i]->imageWidth, tgaFiles[i]->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tgaFiles[i]->imageData);
		if (colorMode == 4) // BGRA
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, tgaFiles[i]->imageWidth, tgaFiles[i]->imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tgaFiles[i]->imageData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#ifndef ANDROID
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	#endif
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	int err = glGetError();
	if (err != 0) {
		log(LOG_ERROR, "OGL error code after creating cube texture");
		return;
	}

	loaded = true;
}


CubeTexture::~CubeTexture()
{
	if (tgaFile)
	{
		delete[] (tgaFile->imageData);
		delete tgaFile;
	}
	glDeleteTextures(1, &mTextureID);
}
