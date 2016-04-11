#ifndef __MESHBLEND_H__
#define __MESHBLEND_H__

#include "Mesh.h"

class MeshBlend: public Mesh
{
public:

	MeshBlend(const char *mesh_file, const char *tex_file);

	virtual void draw(const glm::mat4 &mVPMatrix, const glm::mat4 &mVMatrix);
	virtual void setShader();
	virtual void initShader();

protected:
};



#endif
