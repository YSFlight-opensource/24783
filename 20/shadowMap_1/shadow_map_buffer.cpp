#include <stdio.h>
#include <stdlib.h>

#include "shadow_map_buffer.h"

OpenGLShadowMapBuffer::OpenGLShadowMapBuffer()
{
	frameBufferIdent=0;
	shadowTexIdent=0;
	CleanUp();
}
OpenGLShadowMapBuffer::~OpenGLShadowMapBuffer()
{
	CleanUp();
}
void OpenGLShadowMapBuffer::CleanUp(void)
{
	if(0<frameBufferIdent)
	{
		glDeleteFramebuffers(1,&frameBufferIdent);
	}
	if(0<shadowTexIdent)
	{
		glDeleteTextures(1,&shadowTexIdent);
	}
}
void OpenGLShadowMapBuffer::PrepareBuffer(int shadowTexWid,int shadowTexHei)
{
    glGenTextures(1,&shadowTexIdent);
	glBindTexture(GL_TEXTURE_2D,shadowTexIdent);
#if (!defined(GL_ES) || GL_ES==0) && !defined(GL_ES_VERSION_2_0)
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,shadowTexWid,shadowTexHei,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
#else
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,shadowTexWid,shadowTexHei,0,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,nullptr); // ES needs to use GL_UNSIGNED_INT
#endif
    

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1,&frameBufferIdent);
	glBindFramebuffer(GL_FRAMEBUFFER,frameBufferIdent);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,shadowTexIdent,0);
#if (!defined(GL_ES) || GL_ES==0) && !defined(GL_ES_VERSION_2_0)
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE); // <- Without this, frame buffer status becomes 36060, which seems to be GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
#else
	// OpenGL ES does not allow framebuffer with a depth buffer without color buffer.
	glGenTextures(1,&colorTexIdent);
	glBindTexture(GL_TEXTURE_2D,colorTexIdent);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,shadowTexWid,shadowTexHei,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colorTexIdent,0);
#endif
    
	printf("%d %d\n",frameBufferIdent,shadowTexIdent);

	auto sta=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(sta!=GL_FRAMEBUFFER_COMPLETE)
	{
		switch(sta)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
			break;
		//case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		//	printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
		//	break;
#if (!defined(GL_ES) || GL_ES==0) && !defined(GL_ES_VERSION_2_0)
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
			break;
#endif
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("GL_FRAMEBUFFER_UNSUPPORTED\n");
			break;
		default:
			printf("Unknown error %d\n",sta);
			break;
		}

		printf("Cannot generate a frame buffer.\n");
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	printf("%s %d\n",__FUNCTION__,__LINE__);
}


