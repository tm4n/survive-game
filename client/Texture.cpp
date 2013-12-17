#include "Texture.h"
#include <iostream>
#include "SDL2/SDL.h"


Texture::Texture(const char* filename)
{
	tgaFile = new tga_file;
    unsigned char ucharBad, imagedescriptor;
    short int sintBad;
    long imageSize;
    int colorMode;
    //unsigned char colorSwap;

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
		// read image data from back to front
		int index = (tgaFile->imageHeight-1) * tgaFile->imageWidth;
        for (int y = (tgaFile->imageHeight-1); y >= 0; y--)
		{
			for (int x = 0; x < tgaFile->imageWidth; x++)
			{
				SDL_RWread(file, &(tgaFile->imageData[colorMode*(index + x)]), colorMode, 1);
			}
			index -= tgaFile->imageWidth;
        }
	}
	else
	{
		// Read the image data normally
		SDL_RWread(file, tgaFile->imageData, sizeof(uint8_t), imageSize);
	}

    // Change from BGR to RGB so OpenGL can read the image data.
    /*for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tgaFile->imageData[imageIdx];
        tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
        tgaFile->imageData[imageIdx + 2] = colorSwap;
    }*/

	SDL_RWclose(file);

	// now upload texture to OpenGL
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (colorMode == 3) // BGR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tgaFile->imageWidth, tgaFile->imageHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, tgaFile->imageData);
	if (colorMode == 4) // BGRA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tgaFile->imageWidth, tgaFile->imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, tgaFile->imageData);


	loaded = true;
}

Texture::~Texture()
{
	delete[] (tgaFile->imageData);
	delete tgaFile;
}