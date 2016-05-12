#include <vector>

#include <fslazywindow.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>


class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	int shipX,shipY;

	int missileState;
	int missileX,missileY;

	int nPrevTouch;

public:
	void FireMissile(void);
	void MoveMissile(void);

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


void FsLazyWindowApplication::FireMissile(void)
{
	if(0==missileState)
	{
		missileState=1;
		missileX=shipX;
		missileY=shipY;
	}
}

void FsLazyWindowApplication::MoveMissile(void)
{
	missileY-=10;
	if(0>missileY)
	{
		missileState=0;
	}
}


FsLazyWindowApplication::FsLazyWindowApplication()
{
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

	int wid,hei;
	FsGetWindowSize(wid,hei);

	nPrevTouch=0;

	shipX=wid/2;
	shipY=hei*9/10;

	missileState=0;
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}


	auto nTouch=FsGetNumCurrentTouch();
	auto touch=FsGetCurrentTouch();

	if(1==nTouch)
	{
		if(shipX<touch[0].x()-10)
		{
			shipX+=10;
		}
		if(touch[0].x()+10<shipX)
		{
			shipX-=10;
		}
	}
	else if(nPrevTouch<2 && nTouch==2)
	{
		FireMissile();
	}

	MoveMissile();

	nPrevTouch=nTouch;
	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	needRedraw=false;

	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	{
		YsGLSL2DRenderer renderer;
		renderer.UseWindowCoordinateTopLeftAsOrigin();

		std::vector <GLfloat> vtx;

// 		for(int touchIdx=0; touchIdx<FsGetNumCurrentTouch(); ++touchIdx)
// 		{
// 			auto touch=FsGetCurrentTouch();
// 			auto touchPos=touch[touchIdx];
// 			vtx.push_back(0);
// 			vtx.push_back((float)touchPos.y());
// 			vtx.push_back(wid);
// 			vtx.push_back((float)touchPos.y());
// 
// 			vtx.push_back((float)touchPos.x());
// 			vtx.push_back(0);
// 			vtx.push_back((float)touchPos.x());
// 			vtx.push_back(hei);
// 		}

		GLfloat black[]={0,0,0,1};
		renderer.SetUniformColor(black);
		renderer.DrawVtx(GL_LINES,vtx.size()/2,vtx.data());

		// Ship
		GLfloat shipVtx[16]=
		{
			shipX-4 ,shipY-4,
			shipX-4 ,shipY-12,
			shipX+4 ,shipY-12,
			shipX+4 ,shipY-4,
			shipX+12,shipY-4,
			shipX+12,shipY+4,
			shipX-12,shipY+4,
			shipX-12,shipY-4,
		};
		renderer.DrawVtx(GL_TRIANGLE_FAN,8,shipVtx);

		// Missile
		GLfloat missileVtx[8]=
		{
			missileX-4,missileY,
			missileX  ,missileY-4,
			missileX+4,missileY,
			missileX  ,missileY+4
		};
		renderer.DrawVtx(GL_TRIANGLE_FAN,4,missileVtx);
	}

	FsSwapBuffers();
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
