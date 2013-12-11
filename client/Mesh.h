#ifndef __MESH_H__
#define __MESH_H__

#include "RenderObject.h"
#include <list>
#include <string>
#include "gl_core_2_1.h"
#include "glm/glm.hpp"

class Mesh
{
public:
	bool loaded;

	std::string filename;

	int numskins;
	int numverts;
	int numtris;
	int numframes;

	Mesh(const char *mesh_file, const char *tex_file);

	void draw(glm::mat4 mVPMatrix);

	void initShader();

	void addRenderObject(RenderObject *obj);
    
	void removeRenderObject(RenderObject *obj);

protected:
	char *vertexShaderCode;
	char *fragmentShaderCode;

	std::list<RenderObject*> objectList;

private:
	glm::mat4 mTransformationMatrix;
	glm::mat4 mFinalMatrix;

	GLuint *mTextureID;
    GLuint mProgram;
    GLuint mTexCoordHandle;
    GLuint mPositionHandle;
    GLuint mNextPositionHandle;
    GLuint mAnimProgressHandle;
    GLuint *mVertexBuffer;
    GLuint mTexCoordBuffer;
    GLuint mMVPMatrixHandle;

	int loadShader(int type, const char *code);
};



#endif
