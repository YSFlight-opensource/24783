#include <vector>

#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <fslazywindow.h>

#include "vertex_buffer_object.h"
#include "binary_stl.h"

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	Ys3DDrawingEnvironment drawEnv;


	int prevMx,prevMy;


	std::vector <float> vtx,nom;
	std::vector <float> col;
	YsVec3 min,max;

	YsVec3 lastClick[2];



public:
	void LoadBinaryStl(const char fn[]);
	void CacheBoundingBox(void);

	void MouseCoordinateTo3DLine(YsVec3 ln[2],int mx,int my) const;
	int PickedTriangle(int mx,int my) const;


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

void FsLazyWindowApplication::LoadBinaryStl(const char fn[])
{
	::LoadBinaryStl(vtx,nom,fn);
	col.clear();
	for(int i=0; i<vtx.size()/3; ++i)
	{
		col.push_back(0);
		col.push_back(0);
		col.push_back(1);
		col.push_back(1);
	}
	CacheBoundingBox();
}

void FsLazyWindowApplication::CacheBoundingBox(void)
{
	auto nVtx=vtx.size()/3;
	// Cache bounding box
	if(0<nVtx)
	{
		float minx,miny,minz,maxx,maxy,maxz;
		minx=vtx[0];
		miny=vtx[1];
		minz=vtx[2];
		maxx=vtx[0];
		maxy=vtx[1];
		maxz=vtx[2];
		for(decltype(nVtx) i=0; i<nVtx; ++i)
		{
			YsMakeSmaller(minx,vtx[i*3]);
			YsMakeSmaller(miny,vtx[i*3+1]);
			YsMakeSmaller(minz,vtx[i*3+2]);
			YsMakeGreater(maxx,vtx[i*3]);
			YsMakeGreater(maxy,vtx[i*3+1]);
			YsMakeGreater(maxz,vtx[i*3+2]);
		}
		min.Set(minx,miny,minz);
		max.Set(maxx,maxy,maxz);
	}
	else
	{
		min=YsVec3::Origin();
		max=YsVec3::Origin();
	}
}

void FsLazyWindowApplication::MouseCoordinateTo3DLine(YsVec3 ln[2],int mx,int my) const
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	double x=(double)mx/(double)wid;
	double y=(double)(hei-my)/(double)hei;
	x=x*2.0-1.0;
	y=y*2.0-1.0;

	ln[0].Set(x,y,-1.0);
	ln[1].Set(x,y, 1.0);
	for(int i=0; i<2; ++i)
	{
		auto &p=ln[i];
		drawEnv.GetProjectionMatrix().MulInverse(p,p,1.0);
		drawEnv.GetViewMatrix().MulInverse(p,p,1.0);
	}
}

int FsLazyWindowApplication::PickedTriangle(int mx,int my) const
{
	YsVec3 ln[2];
	MouseCoordinateTo3DLine(ln,mx,my);

	const YsVec3 o=ln[0];
	const YsVec3 v=YsUnitVector(ln[1]-ln[0]);

	int picked=-1;
	double pickedDist=0.0;
	for(int i=0; i<vtx.size()/9; ++i)
	{
		const YsVec3 tri[3]=
		{
			YsVec3(vtx[i*9  ],vtx[i*9+1],vtx[i*9+2]),
			YsVec3(vtx[i*9+3],vtx[i*9+4],vtx[i*9+5]),
			YsVec3(vtx[i*9+6],vtx[i*9+7],vtx[i*9+8]),
		};
		YsPlane pln;
		pln.MakePlaneFromTriangle(tri[0],tri[1],tri[2]);

		YsVec3 itsc;
		if(YSOK==pln.GetIntersection(itsc,o,v))
		{
			auto side=YsCheckInsideTriangle3(itsc,tri);
			if(YSINSIDE==side || YSBOUNDARY==side)
			{
				auto dist=(itsc-o)*v; // Gives distance
				if(0.0<dist && (picked<0 || dist<pickedDist))
				{
					picked=i;
					pickedDist=dist;
				}
			}
		}
	}

	return picked;
}




FsLazyWindowApplication::FsLazyWindowApplication()
{
	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetViewTarget(YsVec3::Origin());
	drawEnv.SetViewDistance(20.0);
	drawEnv.SetViewAttitude(YsAtt3(YsPi/4.0,YsPi/5.0,0.0));

	prevMx=0;
	prevMy=0;

	needRedraw=false;

	lastClick[0]=YsVec3::Origin();
	lastClick[1]=YsVec3::Origin();
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
	if(2<=argc)
	{
		if(true==::IsAsciiStl(argv[1]))
		{
			printf("It is not a binary stl!\n");
			return;
		}
		LoadBinaryStl(argv[1]);
	}
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
	if(evt==FSMOUSEEVENT_LBUTTONDOWN)
	{
		MouseCoordinateTo3DLine(lastClick,mx,my);

		int triIdx=PickedTriangle(mx,my);
		if(0<=triIdx)
		{
			for(int i=0; i<3; ++i)
			{
				col[((3*triIdx)+i)*4  ]=1;
				col[((3*triIdx)+i)*4+1]=0;
				col[((3*triIdx)+i)*4+2]=0;
			}
		}
	}
	

	prevMx=mx;
	prevMy=my;

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
	drawEnv.SetViewTarget((min+max)/2.0);
	drawEnv.SetViewDistance((max-min).GetLength());

	GLfloat projMat[16];
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);


	auto &view=drawEnv.GetViewMatrix();

	YsMatrix4x4 modeling;

	YsMatrix4x4 fullMatrix=view*modeling;

	GLfloat viewMat[16];
	fullMatrix.GetOpenGlCompatibleMatrix(viewMat);

	{
		GLfloat lightDir[]={0,0,1};

		YsGLSLShaded3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);
		renderer.SetLightDirectionInCameraCoordinate(0,lightDir);

		renderer.DrawVtxNomCol(GL_TRIANGLES,vtx.size()/3,vtx.data(),nom.data(),col.data());
	}
	{
		YsGLSLPlain3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);

		GLfloat color[8]={0,0,1,1, 1,0,0,1};
		const GLfloat vtx[6]=
		{
			lastClick[0].xf(),lastClick[0].yf(),lastClick[0].zf(),
			lastClick[1].xf(),lastClick[1].yf(),lastClick[1].zf()
		};
		renderer.DrawVtxCol(GL_LINES,2,vtx,color);
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
		renderer.DrawVtx(GL_TRIANGLES,vtx.size()/3,vtx.data());
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
