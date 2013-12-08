#ifndef __MESH_H__
#define __MESH_H__

#include <list>

class Mesh
{
public:
	int numverts;
	int numtris;
	int numframes;

	Mesh(const char *mesh_file, const char *tex_file);

	void draw();

	void initShader();

protected:
	char *vertexShaderCode;
	char *fragmentShaderCode;

	//std::list<RenderObject> objectList;

private:
	int mTextureID;
    int mProgram;
    int mTexCoordHandle;
    int mPositionHandle;
    int mNextPositionHandle;
    int mAnimProgressHandle;
    int *mVertexBuffer;
    int mTexCoordBuffer;
    int mMVPMatrixHandle;

    //final int vertexStride = 3 * 4; // 4 bytes per vertex
    //final int texCoordsStride = 2 * 4; // 4 bytes per vertex

	int loadShader(int type, const char *code);
};



#endif