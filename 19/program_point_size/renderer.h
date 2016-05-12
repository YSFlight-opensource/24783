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

class Plain3dRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint attribColorPos;
	GLuint uniformProjectionPos;
	GLuint uniformModelViewPos;
	virtual void CacheAttributeAndUniformIdent(void);
};

class Rainbow3dRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint uniformProjectionPos;
	GLuint uniformModelViewPos;
	virtual void CacheAttributeAndUniformIdent(void);
};

class Gouraud3dRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint attribNormalPos;
	GLuint attribColorPos;

	GLuint uniformProjectionPos;
	GLuint uniformModelViewPos;
	GLuint uniformLightDirPos;
	GLuint uniformAmbientPos;
	GLuint uniformSpecularIntensityPos;
	GLuint uniformSpecularExponentPos;

	virtual void CacheAttributeAndUniformIdent(void);
};

class Phong3dRenderer : public Gouraud3dRenderer
{
};


class Billboard3dRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint attribOffsetPos;
	GLuint attribColorPos;
	GLuint attribTexCoordPos;

	GLuint uniformProjectionPos;
	GLuint uniformModelViewPos;
	GLuint uniformOffsetInViewPos;
	GLuint uniformOffsetInPixelPos;
	GLuint uniformViewportWidthPos;
	GLuint uniformViewportHeightPos;
	GLuint uniformTexture2dPos;

	virtual void CacheAttributeAndUniformIdent(void);
};

class ProgramPointSize3dRenderer : public RendererBase
{
public:
	GLuint attribVertexPos;
	GLuint attribColorPos;
	GLuint attribPointSizePos;
	GLuint uniformProjectionPos;
	GLuint uniformModelViewPos;
	virtual void CacheAttributeAndUniformIdent(void);
};

std::vector <char> ReadTextFile(const char fn[]);

#endif
