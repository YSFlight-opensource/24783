#ifndef OPENGL_HEADER_IS_INCLUDED
#define OPENGL_HEADER_IS_INCLUDED

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		// Prevent inclusion of winsock.h
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#undef WIN32_LEAN_AND_MEAN
	#else
		// Too late.  Just include it.
		#include <windows.h>
	#endif
#endif

#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
#endif

#ifndef __APPLE__
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#else
    #include <TargetConditionals.h>
    #if TARGET_OS_EMBEDDED!=0 || TARGET_OS_IPHONE!=0 || TARGET_OS_IPHONE_SIMULATOR!=0
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		typedef double GLdouble;
	#else
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
		#include <OpenGL/glext.h>
	#endif
#endif

#if !defined(GL_PROGRAM_POINT_SIZE) && defined(GL_PROGRAM_POINT_SIZE_EXT)
#define GL_PROGRAM_POINT_SIZE GL_PROGRAM_POINT_SIZE_EXT
#endif

#endif
