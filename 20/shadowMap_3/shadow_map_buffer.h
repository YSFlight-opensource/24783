#ifndef SHADOW_MAP_BUFFER_IS_INCLUDED
#define SHADOW_MAP_BUFFER_IS_INCLUDED
/* { */

#include "opengl_header.h"

class OpenGLShadowMapBuffer
{
public:
	GLuint frameBufferIdent;
	GLuint shadowTexIdent;

	OpenGLShadowMapBuffer();
	~OpenGLShadowMapBuffer();
	void CleanUp(void);

	void PrepareBuffer(int wid,int hei);
};

/* } */
#endif
