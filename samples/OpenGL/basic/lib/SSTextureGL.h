#ifndef __TKTEXTURE__
#define __TKTEXTURE__

#include <windows.h>
#include <stdio.h>

#include <GL/glew.h>
#include "stb_image.h"


class	SSTextureGL
{
public:
	GLuint	tex;
	int tex_width;
	int	tex_height;
	bool texture_is_pow2;

public:
	SSTextureGL() : tex_width(0) , tex_height(0) , tex(0), texture_is_pow2(true){}
	virtual ~SSTextureGL();
	bool Load( const char* fname );	

	int	getWidth() { return tex_width; }
	int	getHeight() { return tex_height; }
	static SSTextureGL* create(){ return new SSTextureGL(); }

};

extern inline bool SsUtTextureisPow2(int n);







#endif
