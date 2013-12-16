#ifndef __GUI_H__
#define __GUI_H__

#include "gl_core_2_1.h";
#include <vector>
#include "Texture.h"
#include "GUIObject.h"

class GUI
{

public:

	GUI();

	void setScreensize(int x, int y);

	int addPanel(Texture *tex, int layer, float x, float y);
	void clear();

	void setVisible(int id, bool vis);
	void setCentered(int id, bool cen);
	void setX(int id, float x);
	void setY(int id, float y);
	void setLayer(int id, int layer);
	void setSizeX(int id, int size_x);
	void setSizeY(int id, int size_y);
	void setScaleX(int id, float scale_x);
	void setScaleY(int id, float scale_y);
	void setAlpha(int id, float alpha);

	void draw();


private:

	int screensize_x, screensize_y;

	GLuint mProgram;
	GLuint mPositionHandle;
	GLuint mChangeHandle;
	GLuint mTexCoordHandle;
	GLuint mAlphaHandle;

	GLuint mVertexBuffer;
	GLuint mTexCoordBuffer;

	static const char *mVertexShaderCode;
	static const char *mFragmentShaderCode;

	int loadShader(int type, const char *shaderCode);

	static float squareCoords[18];
	
	static float squareTexCoords[12];

	std::vector<GUIObject*> elements;
};


#endif