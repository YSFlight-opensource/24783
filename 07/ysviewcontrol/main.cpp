#include <vector>

#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	Ys3DDrawingEnvironment drawEnv;


	int prevMx,prevMy;


	YsAtt3 cubeAtt;
	YsVec3 cubePos;


    std::vector <GLfloat> cubeVtx;


public:
	void MakeCube(double d);
	void DrawPlainCube(YsGLSLShaded3DRenderer &renderer) const;
	void DrawPlainCube(YsGLSLPlain3DRenderer &renderer) const;

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

void FsLazyWindowApplication::MakeCube(double d)
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

	for(auto f : vtxCol)
	{
		cubeVtx.push_back(f);
	}
}

void FsLazyWindowApplication::DrawPlainCube(YsGLSLShaded3DRenderer &renderer) const
{
	renderer.DrawVtxNomCol(GL_TRIANGLES,36,cubeVtx.data(),cubeVtx.data()+108,cubeVtx.data()+216);
}
void FsLazyWindowApplication::DrawPlainCube(YsGLSLPlain3DRenderer &renderer) const
{
	renderer.DrawVtx(GL_TRIANGLES,36,cubeVtx.data());
}

FsLazyWindowApplication::FsLazyWindowApplication()
{
	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetViewTarget(YsVec3::Origin());
	drawEnv.SetViewDistance(20.0);
	drawEnv.SetViewAttitude(YsAtt3(YsPi/4.0,YsPi/5.0,0.0));

	prevMx=0;
	prevMy=0;

	cubeAtt.Set(0.0,0.0,0.0);
	cubePos.Set(0.0,0.0,0.0);

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
	MakeCube(5.0);
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
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


	auto a=(double)FsSubSecondTimer()/1000.0;
	cubePos.Set(2.0*cos(a),0.0,2.0*sin(a));
	cubeAtt.SetH(a);
	cubeAtt.SetP(a*3.0);
	cubeAtt.SetB(a*2.0);


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

	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetAspectRatio((double)wid/(double)hei);
	drawEnv.SetFOVY(YsPi/4.0);
	drawEnv.SetNearFar(0.1,100.0);

	GLfloat projMat[16];
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);


	auto &view=drawEnv.GetViewMatrix();

	YsMatrix4x4 modeling;
	modeling.Translate(cubePos);
	modeling.RotateXZ(cubeAtt.h());
	modeling.RotateZY(cubeAtt.p());
	modeling.RotateXY(cubeAtt.b());

	YsMatrix4x4 fullMatrix=view*modeling;

	GLfloat viewMat[16];
	fullMatrix.GetOpenGlCompatibleMatrix(viewMat);

	{
		GLfloat lightDir[]={0,0,1};

		YsGLSLShaded3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);
		renderer.SetLightDirectionInCameraCoordinate(0,lightDir);
		DrawPlainCube(renderer);
	}

	YsMatrix4x4 shadowMat;
	shadowMat.Translate(0.0,-12.0,0.0);
	shadowMat.Scale(1.0,0.0,1.0);

	fullMatrix=view*shadowMat*modeling;
	fullMatrix.GetOpenGlCompatibleMatrix(viewMat);
	{
		GLfloat color[]={0,0,0,1};

		YsGLSLPlain3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);
		renderer.SetUniformColor(color);
		DrawPlainCube(renderer);
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
