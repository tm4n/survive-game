#include "GUI.h"
#include <iostream>
#include "SDL2/SDL.h"

float GUI::squareCoords[] = { // in counterclockwise order:
        -1.0f,  1.0f, 0.0f,   // top left
       -1.0f, -1.0f, 0.0f,   // bottom left
        1.0f, -1.0f, 0.0f,    // bottom right
        -1.0f,  1.0f, 0.0f,   // top left
        1.0f, -1.0f, 0.0f,    // bottom right
        1.0f,  1.0f, 0.0f   // top left
};
	
float GUI::squareTexCoords[] = { // in counterclockwise order:
        0.0f,  0.0f,  // top left
       0.0f, 1.0f,   // bottom left
        1.0f, 1.0f,    // bottom right
        0.0f,  0.0f,   // top left
        1.0f, 1.0f,    // bottom right
        1.0f,  0.0f   // top right
};

const char *GUI::mVertexShaderCode = 
	"#version 120 \n"
	"attribute vec4 vPosition; \n"
	"uniform vec4 vChange; \n"
	"attribute vec2 TexCoordIn; \n" 
    "varying vec2 TexCoordOut; \n"
	"void main() { \n"
	"  gl_Position = vec4(vec2(vPosition.x*vChange[0]+vChange[1], vPosition.y*vChange[2]-vChange[3]), 0.0, 1.0); \n"
	"  TexCoordOut = TexCoordIn; \n"
	"}";

const char *GUI::mFragmentShaderCode =
	"#version 120 \n"
	"uniform float Alpha; \n" 
	"varying vec2 TexCoordOut; \n"
	"uniform sampler2D Texture; \n" 
	"void main() { \n" 
	"  gl_FragColor = texture2D(Texture, TexCoordOut); \n" 
    "  gl_FragColor.w *= Alpha; \n" 
	"}";


GUI::GUI()
{
	// ready up draw shaders
	// initialize shaders
    int vertexShader = loadShader(GL_VERTEX_SHADER, mVertexShaderCode);


    int fragmentShader = loadShader(GL_FRAGMENT_SHADER, mFragmentShaderCode);

    mProgram = glCreateProgram();             // create empty OpenGL ES Program
    glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
    glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
    glLinkProgram(mProgram);                  // creates OpenGL ES program executables
        
    // get handle to fragment shader's TexCoord member
    mTexCoordHandle = glGetAttribLocation(mProgram, "TexCoordIn");
        
    // get handle to fragment shader's alpha member
    mAlphaHandle = glGetUniformLocation(mProgram, "Alpha");
        
    // get handle to vertex shader's vPosition member
    mPositionHandle = glGetAttribLocation(mProgram, "vPosition");
        
    // get handle to vertex shader's vChange member
    mChangeHandle = glGetUniformLocation(mProgram, "vChange");
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " after compiling GUI shaders" << std::endl;
    }
        
        
    // load into GPU
    // generate buffers
    GLuint *glbuffer = new GLuint[2];
    glGenBuffers(2, glbuffer);
    mVertexBuffer = glbuffer[0];
    mTexCoordBuffer = glbuffer[1];
	delete[] glbuffer;
        
    // upload vertex list
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 3 * 3 * 4, squareCoords, GL_STATIC_DRAW);  
    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    // upload tex coords
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 3 * 4, squareTexCoords, GL_STATIC_DRAW);  
    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " after uploading GUI vertex buffer" << std::endl;
    }
}

int GUI::loadShader(int type, const char *shaderCode)
{
    // create a vertex shader type (GL_VERTEX_SHADER)
    // or a fragment shader type (GL_FRAGMENT_SHADER)
    int shader = glCreateShader(type);

    // add the source code to the shader and compile it
	int len = strlen(shaderCode);
    glShaderSource(shader, 1, &shaderCode, &len);
    glCompileShader(shader);
        
    // Get the compilation status.
    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
     
    // If the compilation failed, delete the shader.
    if (compileStatus == 0)
    {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        std::cout << "GUI Shader compile error: " << strInfoLog << std::endl;
		delete[] strInfoLog;
		
		exit(-3);
    }

    return shader;
}

void GUI::setScreensize(int x, int y)
{
	screensize_x = x;
	screensize_y = y;
}


void GUI::event_mouse(SDL_Event *evt)
{
	if (evt->type == SDL_MOUSEBUTTONDOWN)
	{
		int i = 0;
		for(GUIObject *obj : elements)
		{
			if (obj->type == GUIObject::Types::button && obj->visible == true)
			{
				// check if mouse is in object area
				if (evt->button.x > obj->x*screensize_x && evt->button.x < obj->x*screensize_x+obj->size_x*obj->scale_x &&
					evt->button.y > obj->y*screensize_y && evt->button.y < obj->y*screensize_y+obj->size_y*obj->scale_y)
				{
					if (obj->callback != NULL) obj->callback->callback(i);
						else puts("No callback registered for clicked button");
				}
			}
			i++;
		}
	}
}

float GUI::to_glscreen_x(float pos_x)
{
	return (((pos_x/screensize_x)*2.f)-1.f);
}

float GUI::to_glscreen_y(float pos_y)
{
	return (((pos_y/screensize_y)*2.f)-1.f);
}

void GUI::draw()
{
	// animate buttons
	int raw_x, raw_y;
	SDL_GetMouseState(&raw_x, &raw_y);

	for(GUIObject *obj : elements)
	{
		if (obj->type == GUIObject::Types::button && obj->visible == true)
		{
			// check if mouse is in object area TODO: all kinds of scalings!
			if (raw_x > obj->x*screensize_x && raw_x < obj->x*screensize_x+obj->size_x*obj->scale_x &&
				raw_y > obj->y*screensize_y && raw_y < obj->y*screensize_y+obj->size_y*obj->scale_y)
			{
				obj->current_tex = 1;
			}
			else
			{
				obj->current_tex = 0;
			}
		}
	}




	//////////////////////////////////////////////////////////////////////////////////
	// drawing

	// Disable Depth Testing and activate other polygon order
	glDisable(GL_DEPTH_TEST);  
	glFrontFace(GL_CCW);
		
	// Add program to OpenGL ES environment
	glUseProgram(mProgram);

	// Bind vertex buffer object for triangle vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        
    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                    GL_FLOAT, false,
                                    12, 0);
        
    // get texture mapping into gpu
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
    glEnableVertexAttribArray(mTexCoordHandle);
    glVertexAttribPointer(mTexCoordHandle, 2, 
        						GL_FLOAT, false, 
        						8, 0);
        
    for (int layer = 0; layer < 10; layer++)
    {
	    for (GUIObject *elem : elements)
		{	        	
	        if (elem->layer == layer && elem->visible == true)
	        {				
	        	// bind texture of this element
				glBindTexture(GL_TEXTURE_2D, elem->textures[elem->current_tex]->mTextureID);
	        		
	        	// calculate and upload size and position
				// TODO: auto-scale
				float trans_x;
				float trans_y;
				float size_x = (float)elem->size_x/((float)screensize_x) * elem->scale_x;
				float size_y = (float)elem->size_y/((float)screensize_y) * elem->scale_y;
				switch (elem->alignment)
				{
				case GUIObject::Alignment::center:
					trans_x = to_glscreen_x(elem->x + screensize_x/2);
					trans_y = to_glscreen_y(elem->y + screensize_y/2);
				break;

				case GUIObject::Alignment::upleft:
					trans_x = to_glscreen_x(elem->x);
					trans_y = to_glscreen_y(elem->y);
				break;

				case GUIObject::Alignment::upcenter:
					trans_x = to_glscreen_x(elem->x + screensize_x/2);
					trans_y = to_glscreen_y(elem->y);
				break;

				case GUIObject::Alignment::upright:
					trans_x = to_glscreen_x(elem->x+screensize_x);
					trans_y = to_glscreen_y(elem->y);
				break;

				case GUIObject::Alignment::downright:
					trans_x = to_glscreen_x(elem->x+screensize_x);
					trans_y = to_glscreen_y(elem->y+screensize_y);
				break;

				case GUIObject::Alignment::downcenter:
					trans_x = to_glscreen_x(elem->x + screensize_x/2);
					trans_y = to_glscreen_y(elem->y + screensize_y);
				break;

				case GUIObject::Alignment::downleft:
					trans_x = to_glscreen_x(elem->x);
					trans_y = to_glscreen_y(elem->y+screensize_y);
				break;

				case GUIObject::Alignment::scaled: // TODO:
					trans_x = ((elem->x*2.f)-1.f);
					trans_y = ((elem->y*2.f)-1.f);
					//size_x = 20;
					//size_y = 20;
				break;
					
				}

				if (elem->centered == false) {trans_x += size_x; trans_y += size_y;}
		        	
		        glUniform4f(mChangeHandle, size_x, trans_x, size_y, trans_y);
		        	
		        // Upload element alpha to fragment shader
		        glUniform1f(mAlphaHandle, elem->alpha);
		        	
		    	// Draw the triangle
		    	glDrawArrays(GL_TRIANGLES, 0, 6);
	        }
	    }
    }

	// Disable vertex arrays
	glDisableVertexAttribArray(mPositionHandle);
	glDisableVertexAttribArray(mTexCoordHandle);
	    
	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	    
	// Reenable Depth Testing and normal triangle order
	glEnable(GL_DEPTH_TEST);  
	glFrontFace(GL_CW);
}


int GUI::addPanel(Texture *tex, int layer, GUIObject::Alignment align, float x, float y)
{
	GUIObject *elem = new GUIObject(GUIObject::Types::panel, tex, layer, align, x, y, NULL);
		
	elements.push_back(elem);
		
	return elements.size()-1;
}

int GUI::addButton(Texture *tex, Texture *tex_sel, int layer, GUIObject::Alignment align, float x, float y, GUICallback *callback)
{
	std::vector<Texture*> v;

	v.push_back(tex);
	v.push_back(tex_sel);

	GUIObject *elem = new GUIObject(GUIObject::Types::button, v, layer, align, x, y, callback);

	elements.push_back(elem);
		
	return elements.size()-1;
}

int GUI::addText(Texture *tex, int layer, GUIObject::Alignment align, float x, float y)
{
	GUIObject *elem = new GUIObject(GUIObject::Types::text, tex, layer, align, x, y, NULL);
		
	elements.push_back(elem);
		
	return elements.size()-1;
}

int GUI::addText(const std::string &txt, TTF_Font *fnt, int layer, GUIObject::Alignment align, float x, float y, Uint8 cred, Uint8 cgreen, Uint8 cblue)
{
	SDL_Color c = {cred, cgreen, cblue};
	Texture *t = new Texture(txt, fnt, c);
	
	GUIObject *elem = new GUIObject(GUIObject::Types::text, t, layer, align, x, y, NULL);
	
	elem->txt.assign(txt);
	elem->fnt = fnt;
		
	elements.push_back(elem);
		
	return elements.size()-1;
}

bool GUI::updateText(int id, const std::string &txt, Uint8 cred, Uint8 cgreen, Uint8 cblue)
{
	GUIObject *elem = elements.at(id);
	if (elem == NULL) return false;
	if (elem->type != GUIObject::Types::text) return false;
	if (elem->txt.compare(txt) == 0) return false;
	
	SDL_Color c = {cred, cgreen, cblue};
	Texture *t = new Texture(txt, elem->fnt, c);
	
	elem->txt.assign(txt);
	
	delete elements.at(id)->textures[0];
	elements.at(id)->textures[0] = t;

	// update size
	elements.at(id)->size_x = t->size_x;
	elements.at(id)->size_y = t->size_y;
	
	return true;
}


void GUI::updateTexture(int id, Texture *newtex, int texnum)
{
	delete elements.at(id)->textures[texnum];
	elements.at(id)->textures[texnum] = newtex;

	// update size
	elements.at(id)->size_x = newtex->size_x;
	elements.at(id)->size_y = newtex->size_y;
}


void GUI::setVisible(int id, bool vis)
{
	elements.at(id)->visible = vis;
}
	
void GUI::setCentered(int id, bool cen)
{
	elements.at(id)->centered = cen;
}

void GUI::setAlignment(int id, GUIObject::Alignment align)
{
	elements.at(id)->alignment = align;
}
	
void GUI::setX(int id, float x)
{
	elements.at(id)->x = x;
}
	
void GUI::setY(int id, float y)
{
	elements.at(id)->y = y;
}
	
void GUI::setLayer(int id, int layer)
{
	elements.at(id)->layer = layer;
}
	
void GUI::setSizeX(int id, int size_x)
{
	elements.at(id)->size_x = size_x;
}
	
void GUI::setSizeY(int id, int size_y)
{
	elements.at(id)->size_y = size_y;
}

void GUI::setScaleX(int id, float scale_x)
{
	elements.at(id)->scale_x = scale_x;
}
	
void GUI::setScaleY(int id, float scale_y)
{
	elements.at(id)->scale_y = scale_y;
}
	
void GUI::setAlpha(int id, float alpha)
{
	elements.at(id)->alpha = alpha;
}
