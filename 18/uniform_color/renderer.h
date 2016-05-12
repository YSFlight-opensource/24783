#ifndef RENDERER_IS_INCLUDED
#define RENDERER_IS_INCLUDED

#include "opengl_header.h"
#include <vector>


class RendererBase
{
public:
	GLuint programIdent;
	GLuint vertexShaderIdent,fragmentShaderIdent;

	bool CompileFile(const char vtxShaderFn[],const char fragShaderFn[]);
	bool Compile(const std::vector <char> &vtxShaderSource,const std::vector <char> &fragShaderSource);
protected:
	bool CompileShader(int shaderIdent);
	bool LinkShader(void);
	virtual void CacheAttributeAndUniformIdent(void)=0;
};

class PassThroughRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	virtual void CacheAttributeAndUniformIdent(void);
};

class ColorAsAttributeRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint attribColorPos;
	virtual void CacheAttributeAndUniformIdent(void);
};

class UniformColorRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint uniformColorPos;
	virtual void CacheAttributeAndUniformIdent(void);
};

std::vector <char> ReadTextFile(const char fn[]);

#endif
