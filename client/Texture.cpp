#include "Texture.h"
#include <iostream>
#include <string>
#include "SDL2/SDL.h"
#include "helper.h"


Texture::Texture(const char* filename)
{
	tgaFile = new tga_file;
    unsigned char ucharBad, imagedescriptor;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;

	loaded = false;

	file.assign(filename);

	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if (file == NULL) {std::cout << "ERROR on SDL_RWFromFile while opening texture file: " << filename << std::endl;  return;}

    // Read the two first bytes we don't need.
    SDL_RWread(file, &ucharBad, sizeof(uint8_t), 1);
    SDL_RWread(file, &ucharBad, sizeof(uint8_t), 1);

    // Which type of image gets stored in imageTypeCode.
    SDL_RWread(file, &tgaFile->imageTypeCode, sizeof(uint8_t), 1);

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
        SDL_RWclose(file);
        return;
    }

    // Read 13 bytes of data we don't need.
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);
    SDL_RWread(file, &ucharBad, sizeof(uint8_t), 1);
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);

    // Read the image's width and height.
    SDL_RWread(file, &tgaFile->imageWidth, sizeof(int16_t), 1);
    SDL_RWread(file, &tgaFile->imageHeight, sizeof(int16_t), 1);
	size_x = tgaFile->imageWidth; size_y = tgaFile->imageHeight;

    // Read the bit depth.
    SDL_RWread(file, &tgaFile->bitCount, sizeof(uint8_t), 1);

    // Read one byte of data we don't need.
    SDL_RWread(file, &imagedescriptor, sizeof(uint8_t), 1);

    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tgaFile->bitCount / 8;
    imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;

    // Allocate memory for the image data.
    tgaFile->imageData = new uint8_t[imageSize];

	if( (imagedescriptor & 0x0020) == 0 )
	{
		uint8_t *buf = new uint8_t[imageSize];
		uint32_t prog = 0;
		SDL_RWread(file, buf, sizeof(uint8_t), imageSize);
		
		// read image data from back to front
		int index = (tgaFile->imageHeight-1) * tgaFile->imageWidth;
        for (int y = (tgaFile->imageHeight-1); y >= 0; y--)
		{
			for (int x = 0; x < tgaFile->imageWidth; x++)
			{
				tgaFile->imageData[colorMode*(index + x)] = buf[prog];
				tgaFile->imageData[colorMode*(index + x)+1] = buf[prog+1];
				tgaFile->imageData[colorMode*(index + x)+2] = buf[prog+2];
				if (colorMode >= 4) tgaFile->imageData[colorMode*(index + x)+3] = buf[prog+3];
				prog += colorMode;
				//SDL_RWread(file, &(tgaFile->imageData[colorMode*(index + x)]), colorMode, 1); too slow on android
			}
			index -= tgaFile->imageWidth;
        }
        delete[] buf;
	}
	else
	{
		// Read the image data normally
		SDL_RWread(file, tgaFile->imageData, sizeof(uint8_t), imageSize);
	}

    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tgaFile->imageData[imageIdx];
        tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
        tgaFile->imageData[imageIdx + 2] = colorSwap;
    }

	SDL_RWclose(file);

	// now upload texture to OpenGL
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (colorMode == 3) // BGR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tgaFile->imageWidth, tgaFile->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tgaFile->imageData);
	if (colorMode == 4) // BGRA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tgaFile->imageWidth, tgaFile->imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tgaFile->imageData);


	loaded = true;
}

Texture::Texture(std::string txt, TTF_Font *fnt, SDL_Color c)
{
	loaded = false;
	tgaFile = NULL;
	
	file.assign(txt);
	
	// render text
	SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(fnt, txt.c_str(), c, 1024);
	//SDL_Surface *surface = TTF_RenderUTF8_Solid(fnt, txt.c_str(), c);
	
	if (surface == NULL) {log(LOG_ERROR, "Could not render a TTF text!"); exit(-1);}
	
	// Convert to OpenGl texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int colors = surface->format->BytesPerPixel;
	if (colors == 4) {   // alpha
		if (surface->format->Rmask != 0x000000ff)
		{
			unsigned char *d = (unsigned char*)(surface->pixels);
			for (int imageIdx = 0; imageIdx < surface->w*surface->h*colors; imageIdx += colors)
			{
				unsigned char colorSwap = d[imageIdx];
				d[imageIdx] = d[imageIdx + 2];
				d[imageIdx + 2] = colorSwap;
			}
			
			
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
						
	}
	if (colors == 3) {           // no alpha
		if (surface->format->Rmask != 0x000000ff)
		{
			unsigned char *d = (unsigned char*)(surface->pixels);
			for (int imageIdx = 0; imageIdx < surface->w*surface->h*colors; imageIdx += colors)
			{
				unsigned char colorSwap = d[imageIdx];
				d[imageIdx] = d[imageIdx + 2];
				d[imageIdx + 2] = colorSwap;
			}
			
			
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0,
						GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	}
	if (colors == 1)
	{
		// no alpha, palettized
		SDL_Surface *intermediary = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 
			//0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); original!
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); //little endian

		SDL_BlitSurface(surface, 0, intermediary, 0);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, intermediary->w, intermediary->h, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, intermediary->pixels);
						
		SDL_FreeSurface(intermediary);
	}
						
	size_x = surface->w;
	size_y = surface->h;

	SDL_FreeSurface(surface);
						
	loaded = true;
	
	int err = glGetError();
	if (err != 0) {
		log(LOG_ERROR, "OGL error code after creating text texture");
	}
}


Texture::~Texture()
{
	if (tgaFile)
	{
		delete[] (tgaFile->imageData);
		delete tgaFile;
	}
	glDeleteTextures(1, &mTextureID);
}
