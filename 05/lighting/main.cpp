#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	double h,p,b;
	YsVec3 viewTarget;
	double viewDist;


public:
	void DrawPlainCube(YsGLSLShaded3DRenderer &renderer,double d) const;

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

void FsLazyWindowApplication::DrawPlainCube(YsGLSLShaded3DRenderer &renderer,double d) const
{
	GLfloat df=(GLfloat)d;
	GLfloat vtxCol[]=
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

		0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1,  // 36 vertices x 3 floats = 108 floats
		0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 0,0, 1, 
		0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0, 
		0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 0, 1,0, 
		-1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, -1,0,0, 
		 1,0,0,  1,0,0,  1,0,0,  1,0,0,  1,0,0,  1,0,0, 

		1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, // Colors
		1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 1,0,0,1, 
		0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 
		0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 
		0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 
		0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 0,0,1,1, 
	};
	renderer.DrawVtxNomCol(GL_TRIANGLES,36,vtxCol,vtxCol+108,vtxCol+216);
}

FsLazyWindowApplication::FsLazyWindowApplication()
{
	h=YsPi/4.0;
	p=YsPi/5.0;
	b=0;
	viewTarget.Set(0.0,0.0,0.0);
	viewDist=20.0;

	needRedraw=false;
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
	YsGLSLRenderer::CreateSharedRenderer();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	double t=(double)FsSubSecondTimer()/1000.0;
	h=YsPi*t;
	p=sin(YsPi*t)*YsPi/4.0;

	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	YsGLSLRenderer::DeleteSharedRenderer();
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	YsProjectionTransformation proj;
	proj.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	proj.SetAspectRatio((double)wid/(double)hei);
	proj.SetFOVY(YsPi/4.0);
	proj.SetNearFar(0.1,100.0);

	GLfloat projMat[16];
	proj.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);


	YsMatrix4x4 view;
	view.Translate(0,0,-viewDist);
	view.RotateXY(-b);
	view.RotateZY(-p);
	view.RotateXZ(-h);
	view.Translate(-viewTarget);

	GLfloat viewMat[16];
	view.GetOpenGlCompatibleMatrix(viewMat);

	{
		GLfloat lightDir[]={0,0,1};

		YsGLSLShaded3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);
		renderer.SetLightDirectionInCameraCoordinate(0,lightDir);
		DrawPlainCube(renderer,5.0);
	}

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
