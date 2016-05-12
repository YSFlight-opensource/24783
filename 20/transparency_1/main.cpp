#include <stdio.h>
#include <ysclass.h>
#include <ysport.h>

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

	// Cut & Pasted from stl viewer >>
	Ys3DDrawingEnvironment drawEnv;

	int prevMx,prevMy;
	// Cut & Pasted from stl viewer <<


public:
	void DrawCube(double d,GLfloat r,GLfloat g,GLfloat b,GLfloat a);

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

void FsLazyWindowApplication::DrawCube(double d,GLfloat r,GLfloat g,GLfloat b,GLfloat a)
{
	const GLfloat df=(GLfloat)d;
	const GLfloat vtx[]=
	{
		-df,-df,-df,  // 6 quads split into all triangles = 12 triangles
		 df,-df,-df,  // 12 triangles times 3 vertices = 36 vertices
		 df, df,-df,  // 36 vertices times 3 components = 108 floats

		 df, df,-df,
		-df, df,-df,
		-df,-df,-df,

		-df,-df, df,
		 df,-df, df,
		 df, df, df,

		 df, df, df,
		-df, df, df,
		-df,-df, df,

		-df,-df,-df,
		 df,-df,-df,
		 df,-df, df,

		 df,-df, df,
		-df,-df, df,
		-df,-df,-df,
           
		-df, df,-df,
		 df, df,-df,
		 df, df, df,

		 df, df, df,
		-df, df, df,
		-df, df,-df,

		-df,-df,-df,
		-df, df,-df,
		-df, df, df,

		-df, df, df,
		-df,-df, df,
		-df,-df,-df,
           
		 df,-df,-df,
		 df, df,-df,
		 df, df, df,

		 df, df, df,
		 df,-df, df,
		 df,-df,-df,
	};
	const GLfloat nom[]=
	{
		0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,  // 36 vertices x 3 floats = 108 floats
		0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 
		0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 
		0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 
		-1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, 
		 1,0,0,  1,0,0,  1,0,0,  1,0,0,  1,0,0,  1,0,0, 
	};

	glUseProgram(phong3d.programIdent);

	glEnableVertexAttribArray(phong3d.attribVertexPos);
	glEnableVertexAttribArray(phong3d.attribNormalPos);

	glVertexAttribPointer(phong3d.attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx);
	glVertexAttribPointer(phong3d.attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom);

	glDisableVertexAttribArray(phong3d.attribColorPos);
	glVertexAttrib4f(phong3d.attribColorPos,r,g,b,a); // Generic attribute

	glDrawArrays(GL_TRIANGLES,0,36);

	glDisableVertexAttribArray(phong3d.attribVertexPos);
	glDisableVertexAttribArray(phong3d.attribNormalPos);
	glDisableVertexAttribArray(phong3d.attribColorPos);
}

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

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

	glUseProgram(phong3d.programIdent);
	glUniformMatrix4fv(phong3d.uniformProjectionPos,1,GL_FALSE,projMat);
	glUniformMatrix4fv(phong3d.uniformModelViewPos,1,GL_FALSE,viewMat);
	glUniform3f(phong3d.uniformLightDirPos,0,0,1);
	glUniform1f(phong3d.uniformAmbientPos,0.3f);
	glUniform1f(phong3d.uniformSpecularIntensityPos,1);
	glUniform1f(phong3d.uniformSpecularExponentPos,100.0f);

	// What if I draw the transparent cube first?
	DrawCube(5.0,0,0,1,1);
	DrawCube(8.0,1,0,0,0.5);

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
