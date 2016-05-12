#include <stdio.h>
#include <ysclass.h>
#include <ysport.h>
#include <ysbitmap.h>

#include <ysshellext.h>
#include <ysshellextio.h>

#include <fslazywindow.h>
#include "renderer.h"
#include "particle.h"

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

	int nParticle;
	float particleSize;
	ParticleGroup particleGroup;

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
	if(YSOK==bmp.LoadPng("sprite4x4.png"))
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

	nParticle=2000;
	particleSize=2.0f;
	particleGroup.MakeCloud(nParticle,YsVec3(-8,-4,-8),YsVec3(8,4,8));

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
	particleGroup.MakeVertexBuffer(drawEnv.GetViewDirection(),particleSize);

	int wid,hei;
	FsGetWindowSize(wid,hei);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2,0.2,0.8,0.0);
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


	glUseProgram(pointSprite.programIdent);
	glUniformMatrix4fv(pointSprite.uniformProjectionPos,1,GL_FALSE,projMat);
	glUniformMatrix4fv(pointSprite.uniformModelViewPos,1,GL_FALSE,viewMat);

	glUniform1f(pointSprite.uniformSizeInPixelPos,0.0f);
	glUniform1f(pointSprite.uniformSizeIn3dPos,1.0f);
	glUniform1f(pointSprite.uniformViewportHeightPos,(float)hei);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureIdent);
	glUniform1f(pointSprite.uniformTexture2dPos,0);

	glEnableVertexAttribArray(pointSprite.attribVertexPos);
	glEnableVertexAttribArray(pointSprite.attribColorPos);
	glEnableVertexAttribArray(pointSprite.attribPointSizePos);
	glEnableVertexAttribArray(pointSprite.attribTexCoordRangePos);

	glVertexAttribPointer(pointSprite.attribVertexPos,3,GL_FLOAT,GL_FALSE,0,particleGroup.vtx.data());
	glVertexAttribPointer(pointSprite.attribColorPos,4,GL_FLOAT,GL_FALSE,0,particleGroup.col.data());
	glVertexAttribPointer(pointSprite.attribPointSizePos,1,GL_FLOAT,GL_FALSE,0,particleGroup.pointSize.data());
	glVertexAttribPointer(pointSprite.attribTexCoordRangePos,4,GL_FLOAT,GL_FALSE,0,particleGroup.texCoordRange.data());

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS,0,particleGroup.particle.size());
	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_POINT_SPRITE);

	glDisableVertexAttribArray(pointSprite.attribVertexPos);
	glDisableVertexAttribArray(pointSprite.attribColorPos);
	glDisableVertexAttribArray(pointSprite.attribTexCoordRangePos);



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
