#include "SSTextureGL.h"

// nが2のべき乗かどうかチェックする
inline bool SsUtTextureisPow2(int n)
{
	for (int i = 0; i < 16; i++)
	{
		if (n == (1 << i)) return true;
	}
	return false;
}

/* =====================================================================================
	テクスチャファイルの読み込み
===================================================================================== */
GLuint	LoadTextureGL( const char* Filename ,int& width , int& height)
{

	int bpp;
	stbi_uc* image = stbi_load( Filename, &width , &height , &bpp , 0 );
	if ( image == 0 ) return 0;

	int target = GL_TEXTURE_RECTANGLE_ARB;	//glewの機能なのでコメントにする

	if (SsUtTextureisPow2(width) &&
		SsUtTextureisPow2(height))
	{
		target = GL_TEXTURE_2D;
	}


	GLuint glyphTexture = 0;
	glGenTextures(1, &glyphTexture);
	glBindTexture(target, glyphTexture);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if ( bpp == 4 )
	{
/*
		//Ver6 ではストレートアルファで処理を行うのでコメントにする
		stbi_uc *ip = image;
		for ( int i = 0 ; i < width * height ; i++ )
		{
			stbi_uc* r = ip; ip ++;
			stbi_uc* g = ip; ip ++;
			stbi_uc* b = ip; ip ++;
			stbi_uc* a = ip; ip ++;
//			if ( *a == 0 )
			{
				//*r = *g = *b = 0xff;
				int _a = *a;
				int _r = *r;
				int _g = *g;
				int _b = *b;
				*r = ( _r * _a) >> 8 ;
				*g = ( _g * _a) >> 8 ;
				*b = ( _b * _a) >> 8 ;
			}
		}
*/
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);	
	}else if ( bpp == 3 )
	{
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);	
	}
	stbi_image_free (image);

	return glyphTexture;
}



SSTextureGL::~SSTextureGL()
{
	glDeleteTextures( 1 , &tex );
}

bool SSTextureGL::Load( const char* fname )
{
	//int tex_width;
	//int tex_height;

	tex = LoadTextureGL( fname , tex_width , tex_height );
	if (SsUtTextureisPow2(tex_width) &&
		SsUtTextureisPow2(tex_height))
	{
		texture_is_pow2 = true;
	}
	else
	{
		texture_is_pow2 = false;
	}
	return tex != 0;
}
