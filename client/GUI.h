#ifndef __GUI_H__
#define __GUI_H__

#include "gl_core_2_1.h"
#include <vector>
#include "Texture.h"
#include "GUIObject.h"
#include "SDL2/SDL.h"
#include "flist.h"

class GUI
{

public:

	GUI();

	void setScreensize(int x, int y);

	int addPanel(Texture *tex, int layer, GUIObject::Alignment align, float x, float y);
	int addButton(Texture *tex, Texture *tex_sel, int layer, GUIObject::Alignment align, float x, float y, GUICallback *callback);
	int addText(Texture *tex, int layer, GUIObject::Alignment align, float x, float y);
	int addText(const std::string &txt, TTF_Font *fnt, int layer, GUIObject::Alignment align, float x, float y, Uint8 cred = 255, Uint8 cgreen = 255, Uint8 cblue = 255);
	bool removeObject(int id);
	void clear();

	void updateTexture(int id, Texture *newtex, int texnum = 0);
	bool updateText(int id, const std::string &txt, Uint8 cred = 255, Uint8 cgreen = 255, Uint8 cblue = 255);
	void setVisible(int id, bool vis);
	void setCentered(int id, bool cen);
	void setAlignment(int id, GUIObject::Alignment align);
	void setX(int id, float x);
	void setY(int id, float y);
	void setLayer(int id, int layer);
	void setSizeX(int id, int size_x);
	void setSizeY(int id, int size_y);
	void setScaleX(int id, float scale_x);
	void setScaleY(int id, float scale_y);
	void setAlpha(int id, float alpha);

	float getAlpha(int id);

	void draw();
	void event_mouse(SDL_Event *evt);

	int screensize_x, screensize_y;

private:
	float to_glscreen_x(float pos_x);
	float to_glscreen_y(float pos_y);

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

	flist<GUIObject> elements;
};


#endif
