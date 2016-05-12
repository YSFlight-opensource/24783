#include <stdio.h>
#include "renderer.h"


std::vector <char> ReadTextFile(const char fn[])
{
	std::vector <char> fileContent;
	char str[256];

	FILE *fp=fopen(fn,"r");
	if(nullptr!=fp)
	{
		while(nullptr!=fgets(str,255,fp))
		{
			for(int i=0; str[i]!=0; ++i)
			{
				fileContent.push_back(str[i]);
			}
		}
		fclose(fp);
	}
	fileContent.push_back(0);
	return fileContent;
}


////////////////////////////////////////////////////////////

bool RendererBase::CompileFile(const char vtxShaderFn[],const char fragShaderFn[])
{
	auto vtxShaderSource=ReadTextFile(vtxShaderFn);
	auto fragShaderSource=ReadTextFile(fragShaderFn);

	printf("Vertex Shader\n");
	printf("%s\n",vtxShaderSource.data());

	printf("Fragment Shader\n");
	printf("%s\n",fragShaderSource.data());

	return Compile(vtxShaderSource,fragShaderSource);
}

bool RendererBase::Compile(const std::vector <char> &vtxShaderSource,const std::vector <char> &fragShaderSource)
{
	bool res=true;

	vertexShaderIdent=glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderIdent=glCreateShader(GL_FRAGMENT_SHADER);
	programIdent=glCreateProgram();

	const char *vtxShaderSourcePtr=vtxShaderSource.data();
	const char *fragShaderSourcePtr=fragShaderSource.data();
	glShaderSource(vertexShaderIdent,1,&vtxShaderSourcePtr,NULL);
	glShaderSource(fragmentShaderIdent,1,&fragShaderSourcePtr,NULL);

	if(true!=CompileShader(vertexShaderIdent))
	{
		res=false;
	}
	if(true!=CompileShader(fragmentShaderIdent))
	{
		res=false;
	}
	if(true!=LinkShader())
	{
		res=false;
	}

	CacheAttributeAndUniformIdent();

	return res;
}

bool RendererBase::CompileShader(int shaderIdent)
{
	int compileSta=99999,infoLogLength=99999,acquiredErrMsgLen=99999;
	int linkSta=99999;
	const int errMsgLen=1024;
	char errMsg[1024];

	glCompileShader(shaderIdent);

	glGetShaderiv(shaderIdent,GL_COMPILE_STATUS,&compileSta);
	glGetShaderiv(shaderIdent,GL_INFO_LOG_LENGTH,&infoLogLength);
	glGetShaderInfoLog(shaderIdent,errMsgLen-1,&acquiredErrMsgLen,errMsg);
	printf("Compile Status %d Info Log Length %d Error Message Length %d\n",compileSta,infoLogLength,acquiredErrMsgLen);

	if(GL_TRUE!=compileSta)
	{
		int i;
		printf("Error Message: \n%s\n",errMsg);
		return false;
	}
	return true;
}
bool RendererBase::LinkShader(void)
{
	int compileSta=99999,infoLogLength=99999,acquiredErrMsgLen=99999;
	int linkSta=99999;
	const int errMsgLen=1024;
	char errMsg[1024];

	glAttachShader(programIdent,vertexShaderIdent);
	glAttachShader(programIdent,fragmentShaderIdent);
	glLinkProgram(programIdent);
	glGetProgramiv(programIdent,GL_LINK_STATUS,&linkSta);
	glGetProgramiv(programIdent,GL_INFO_LOG_LENGTH,&infoLogLength);
	glGetProgramInfoLog(programIdent,errMsgLen-1,&acquiredErrMsgLen,errMsg);
	printf("Link Status %d Info Log Length %d Error Message Length %d\n",linkSta,infoLogLength,acquiredErrMsgLen);

	if(GL_TRUE!=linkSta)
	{
		printf("Error Message: \n%s\n",errMsg);
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////

/* virtual */ void PassThroughRenderer::CacheAttributeAndUniformIdent(void)
{
	attribVertexPos=glGetAttribLocation(programIdent,"vertex");
	printf("Attribute Vertex Position=%d\n",attribVertexPos);
}
