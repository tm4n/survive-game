#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "gl_core_2_1.h"
#include <stdint.h>
#include <string>

typedef struct
{
    uint8_t imageTypeCode;
    int16_t imageWidth;
    int16_t imageHeight;
    uint8_t bitCount;
    uint8_t *imageData;

} tga_file;


class Texture {

public:
	bool loaded;

	std::string file;

	int size_x, size_y;

	GLuint mTextureID;

	Texture(const char* filename);
	~Texture();

private:
	tga_file *tgaFile;
};


#endif