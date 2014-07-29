#ifndef __PARTICLEMGR_H__
#define __PARTICLEMGR_H__

#include "Texture.h"
#include "vec.h"
#include <list>
#include <vector>
#include "defs.h"
#include "flist.h"
#include "ResourceLoader.h"

// forward declaration
typedef struct particle_data particle_data;

typedef void (*partcb)(particle_data *pdata, float time_delta);
typedef void (*partinit)(particle_data *pdata);

typedef struct particle_data {
	vec pos;
	vec vel;
	float gravity;
	float size;
	float alpha;
	float lifespan;
	ResourceLoader::texType tex;
	partcb pcb;
	
} particle_data;


class particlemgr
{
public:
	ResourceLoader *resources;

	particlemgr(ResourceLoader *resources);
	virtual ~particlemgr();

	void draw(double time_frame, glm::mat4 mVPMatrix, glm::vec3 CamPos);

	void emit(int num, ResourceLoader::texType tex, partcb fnc, partinit initfnc, const vec *pos, const vec *vel);


protected:
	std::list<particle_data> particles;
	std::vector<int> texs;

	GLuint mProgram;
	GLuint mPositionHandle;
	GLuint mAlphaHandle;
	GLuint mSizeHandle;
	GLuint mMVPMatrixHandle;
	GLuint mTexCoordHandle;

	GLuint mVertexBuffer;
	GLuint mTexCoordBuffer;

	static const char *mVertexShaderCode;
	static const char *mFragmentShaderCode;

	static float squareCoords[18];
	static float squareTexCoords[12];

	int loadShader(int type, const char *shaderCode);
};

#endif