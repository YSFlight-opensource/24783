#include <stdio.h>
#include <ysclass.h>
#include <ysport.h>
#include <ysbitmap.h>

#include <ysshellext.h>
#include <ysshellextio.h>

#include <fslazywindow.h>
#include "renderer.h"

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	ColorAsAttributeRenderer colorAsAttrib;
	UniformColorRenderer uniformColor;
	Plain3dRenderer plain3d;
	Rainbow3dRenderer rainbow3d;
	Gouraud3dRenderer gouraud3d;
	Phong3dRenderer phong3d;
	Billboard3dRenderer billboard3d;
	ProgramPointSize3dRenderer programPointSize;
	PointSprite3dRenderer pointSprite;

	// Cut & Pasted from stl viewer >>
	Ys3DDrawingEnvironment drawEnv;

	std::vector <float> cubeVtx;

	std::vector <float> billboardVtx;
	std::vector <float> billboardOffset;
	std::vector <float> billboardColor;
	std::vector <float> billboardTexCoord;

	GLuint textureIdent;

	int prevMx,prevMy;
	// Cut & Pasted from stl viewer <<


public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;

	// Cut & Pasted from stl viewer >>
	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetViewTarget(YsVec3::Origin());
	drawEnv.SetViewDistance(20.0);
	drawEnv.SetViewAttitude(YsAtt3(YsPi/4.0,YsPi/5.0,0.0));

	prevMx=0;
	prevMy=0;

	needRedraw=false;
	// Cut & Pasted from stl viewer <<
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	FsChangeToProgramDir();

	glGenTextures(1,&textureIdent);
	YsBitmap bmp;
	if(YSOK==bmp.LoadPng("BlueImpulse.png"))
	{
		printf("Texture Loaded.\n");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,textureIdent);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,bmp.GetWidth(),bmp.GetHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,bmp.GetRGBABitmapPointer());
	}

	colorAsAttrib.CompileFile(
	    "color_as_attribute_vertex_shader.glsl",
	    "color_as_attribute_fragment_shader.glsl");
	uniformColor.CompileFile(
	    "uniform_color_vertex_shader.glsl",
	    "uniform_color_fragment_shader.glsl");
	plain3d.CompileFile(
		"plain3d_vertex_shader.glsl",
		"plain3d_fragment_shader.glsl");
	rainbow3d.CompileFile(
		"rainbow3d_vertex_shader.glsl",
		"rainbow3d_fragment_shader.glsl");
	gouraud3d.CompileFile(
		"gouraud_vertex_shader.glsl",
		"gouraud_fragment_shader.glsl");
	phong3d.CompileFile(
		"phong_vertex_shader.glsl",
		"phong_fragment_shader.glsl");
	billboard3d.CompileFile(
		"billboard3d_vertex_shader.glsl",
		"billboard3d_fragment_shader.glsl");
	programPointSize.CompileFile(
		"program_point_size_vertex_shader.glsl",
		"program_point_size_fragment_shader.glsl");
	pointSprite.CompileFile(
		"pointsprite_vertex_shader.glsl",
		"pointsprite_fragment_shader.glsl");
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	// Cut & Pasted from stl viewer >>
	int wid,hei;
	FsGetWindowSize(wid,hei);

	int lb,mb,rb,mx,my;
	auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(0!=lb && (mx!=prevMx || my!=prevMy))
	{
		double denom=(double)YsGreater(wid,hei);
		double dx=2.0*(double)(prevMx-mx)/denom;
		double dy=2.0*(double)(prevMy-my)/denom;
		drawEnv.RotateView(dx,dy);
	}
	prevMx=mx;
	prevMy=my;
	// Cut & Pasted from stl viewer <<

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetAspectRatio((double)wid/(double)hei);
	drawEnv.SetFOVY(YsPi/4.0);
	drawEnv.SetNearFar(0.1,100.0);
	drawEnv.SetViewTarget(YsVec3::Origin());
	drawEnv.SetViewDistance(20.0);

	GLfloat projMat[16];
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);

	GLfloat viewMat[16];
	drawEnv.GetViewMatrix().GetOpenGlCompatibleMatrix(viewMat);


	GLfloat cubeEdgeVtx[]=
	{
		-10,-10,-10,  10,-10,-10,
		 10,-10,-10,  10,-10, 10,
		 10,-10, 10, -10,-10, 10,
		-10,-10, 10, -10,-10,-10,

		-10, 10,-10,  10, 10,-10,
		 10, 10,-10,  10, 10, 10,
		 10, 10, 10, -10, 10, 10,
		-10, 10, 10, -10, 10,-10,

		-10, 10,-10, -10,-10,-10,
		 10, 10,-10,  10,-10,-10,
		 10, 10, 10,  10,-10, 10,
		-10, 10, 10, -10,-10, 10,
	};
	GLfloat cubeEdgeColor[]=
	{
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,

		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,

		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
		0,0,0,1, 0,0,0,1,
	};

	glUseProgram(plain3d.programIdent);
	glUniformMatrix4fv(plain3d.uniformProjectionPos,1,GL_FALSE,projMat);
	glUniformMatrix4fv(plain3d.uniformModelViewPos,1,GL_FALSE,viewMat);

	glEnableVertexAttribArray(plain3d.attribVertexPos);
	glEnableVertexAttribArray(plain3d.attribColorPos);

	glVertexAttribPointer(plain3d.attribVertexPos,3,GL_FLOAT,GL_FALSE,0,cubeEdgeVtx);
	glVertexAttribPointer(plain3d.attribColorPos,4,GL_FLOAT,GL_FALSE,0,cubeEdgeColor);

	glDrawArrays(GL_LINES,0,24);

	glDisableVertexAttribArray(plain3d.attribVertexPos);
	glDisableVertexAttribArray(plain3d.attribColorPos);



	GLfloat quadVtx[12]=
	{
		-10,-10,-10,
		 10,-10,-10,
		 10, 10,-10,
		-10, 10,-10,
	};
	GLfloat quadCol[16]=
	{
		1,0,0,1,
		0,1,0,1,
		0,0,1,1,
		1,0,1,1
	};
	GLfloat quadPointSize[4]=
	{
		1,1,2,2
	};
	glUseProgram(pointSprite.programIdent);
	glUniformMatrix4fv(pointSprite.uniformProjectionPos,1,GL_FALSE,projMat);
	glUniformMatrix4fv(pointSprite.uniformModelViewPos,1,GL_FALSE,viewMat);

	glUniform1f(pointSprite.uniformSizeInPixelPos,0.0f);
	glUniform1f(pointSprite.uniformSizeIn3dPos,1.0f);
	glUniform1f(pointSprite.uniformViewportHeightPos,(float)hei);

	glEnableVertexAttribArray(pointSprite.attribVertexPos);
	glEnableVertexAttribArray(pointSprite.attribColorPos);
	glEnableVertexAttribArray(pointSprite.attribPointSizePos);

	glVertexAttribPointer(pointSprite.attribVertexPos,3,GL_FLOAT,GL_FALSE,0,quadVtx);
	glVertexAttribPointer(pointSprite.attribColorPos,4,GL_FLOAT,GL_FALSE,0,quadCol);
	glVertexAttribPointer(pointSprite.attribPointSizePos,1,GL_FLOAT,GL_FALSE,0,quadPointSize);

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS,0,4);
	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_POINT_SPRITE);

	glDisableVertexAttribArray(pointSprite.attribVertexPos);
	glDisableVertexAttribArray(pointSprite.attribColorPos);
	



	FsSwapBuffers();
	needRedraw=false;
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
}


static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}
