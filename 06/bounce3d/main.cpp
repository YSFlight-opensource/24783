#include <vector>

#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <fslazywindow.h>

#include "sphereutil.h"
#include "vertex_buffer_object.h"

class Ball
{
public:
	GLfloat col[4];
	YsVec3 pos,vel;

	void Move(const double dt)
	{
		const double G=9.8;
		pos+=vel*dt;

		YsVec3 a(0.0,-G,0.0);
		vel+=a*dt;
	}
	void BounceOnWall(const YsVec3 &o,const YsVec3 &n)
	{
		const double radius=1.0;
		const double dist=(pos-o)*n;
		if(dist<radius && vel*n<0.0)
		{
			vel-=2.0*n*(vel*n);
		}
	}
};

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	enum 
	{
		NUM_BALL=100
	};
	Ball ball[NUM_BALL];

	bool needRedraw;

	double h,p,b;
	YsVec3 viewTarget;
	double viewDist;


	int prevMx,prevMy;

	int nBallVtx;
	VertexBufferObjectVtxNomCol ballVbo;


public:
	void MakeBall(void);
	void SetInitialLocationAndVelocity(void);
	void Move(const double dt);
	void BallCollision(void);
	void DrawPlainBall(YsGLSLShaded3DRenderer &renderer) const;
	void DrawPlainBall(YsGLSLPlain3DRenderer &renderer) const;

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

void FsLazyWindowApplication::MakeBall(void)
{
	auto sphereVtx=MakeSphere<GLfloat>(12);

	nBallVtx=sphereVtx.size()/3;
	ballVbo.CreateBuffer(2*sizeof(GLfloat)*sphereVtx.size());
	auto currentPtr=ballVbo.GetZeroPointer();
	ballVbo.vtxPtr=ballVbo.PushBufferSubData(currentPtr,sphereVtx.size(),sphereVtx.data());
	ballVbo.nomPtr=ballVbo.PushBufferSubData(currentPtr,sphereVtx.size(),sphereVtx.data());
}

void FsLazyWindowApplication::SetInitialLocationAndVelocity(void)
{
	for(auto &b : ball)
	{
		int x=rand()%21-10;
		int y=10+rand()%21-10;
		int z=rand()%21-10;

		int vx=rand()%21-10;
		int vy=rand()%21-10;
		int vz=rand()%21-10;

		b.pos.Set(x,y,z);
		b.vel.Set(vx,vy,vz);

		switch(rand()%3)
		{
		default:
		case 0:
			b.col[0]=0;
			b.col[1]=0;
			b.col[2]=1;
			b.col[3]=1;
			break;
		case 1:
			b.col[0]=0;
			b.col[1]=1;
			b.col[2]=0;
			b.col[3]=1;
			break;
		case 2:
			b.col[0]=1;
			b.col[1]=0;
			b.col[2]=0;
			b.col[3]=1;
			break;
		}
	}
}

void FsLazyWindowApplication::Move(const double dt)
{
	for(auto &b : ball)
	{
		b.Move(dt);
		b.BounceOnWall(YsVec3(0.0,0.0,0.0),YsYVec());
		b.BounceOnWall(YsVec3( 20.0,0.0,0.0),YsVec3(-1.0,0.0,0.0));
		b.BounceOnWall(YsVec3(-20.0,0.0,0.0),YsVec3( 1.0,0.0,0.0));
		b.BounceOnWall(YsVec3(0.0, 20.0,0.0),YsVec3(0.0,-1.0,0.0));
		b.BounceOnWall(YsVec3(0.0,-20.0,0.0),YsVec3(0.0, 1.0,0.0));
		b.BounceOnWall(YsVec3(0.0,0.0, 20.0),YsVec3(0.0,0.0,-1.0));
		b.BounceOnWall(YsVec3(0.0,0.0,-20.0),YsVec3(0.0,0.0, 1.0));
	}
}

void FsLazyWindowApplication::BallCollision(void)
{
	for(int i=0; i<NUM_BALL; ++i)
	{
		for(int j=i+1; j<NUM_BALL; ++j)
		{
			auto &ball0=ball[i];
			auto &ball1=ball[j];

			auto relVel=ball1.vel-ball0.vel;
			auto relPos=ball1.pos-ball0.pos;
			if(relPos.GetLength()<2.0 && relPos*relVel<0.0)
			{
				auto wallPos=(ball0.pos+ball1.pos)/2.0;
				auto wallNom=YsUnitVector(ball1.pos-ball0.pos);
				ball1.BounceOnWall(wallPos,wallNom);
				ball0.BounceOnWall(wallPos,-wallNom);
			}
		}
	}
}

void FsLazyWindowApplication::DrawPlainBall(YsGLSLShaded3DRenderer &renderer) const
{
	glBindBuffer(GL_ARRAY_BUFFER,ballVbo.GetVboIdent());
	renderer.DrawVtxNom(GL_TRIANGLES,nBallVtx,(GLfloat *)ballVbo.vtxPtr,(GLfloat *)ballVbo.nomPtr);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void FsLazyWindowApplication::DrawPlainBall(YsGLSLPlain3DRenderer &renderer) const
{
	glBindBuffer(GL_ARRAY_BUFFER,ballVbo.GetVboIdent());
	renderer.DrawVtx(GL_TRIANGLES,nBallVtx,(GLfloat *)ballVbo.vtxPtr);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

FsLazyWindowApplication::FsLazyWindowApplication()
{
	h=YsPi/4.0;
	p=YsPi/5.0;
	b=0;
	viewTarget.Set(0.0,10.0,0.0);
	viewDist=20.0;

	prevMx=0;
	prevMy=0;

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
	MakeBall();
	SetInitialLocationAndVelocity();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto dt=(double)FsPassedTime()/1000.0;
	Move(dt);
	BallCollision();

	int wid,hei;
	FsGetWindowSize(wid,hei);

	int lb,mb,rb,mx,my;
	auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(0!=lb && (mx!=prevMx || my!=prevMy))
	{
		double denom=(double)YsGreater(wid,hei);
		double dx=2.0*(double)(prevMx-mx)/denom;
		double dy=2.0*(double)(prevMy-my)/denom;

		YsVec3 fv(0,0,-1),uv(0,1,0);
		fv.RotateXZ(-dx);
		fv.RotateZY(-dy);
		uv.RotateXZ(-dx);
		uv.RotateZY(-dy);

		YsMatrix3x3 view; // Camera to World
		view.RotateXZ(h);
		view.RotateZY(p);
		view.RotateXY(b);
		fv=view*fv;
		uv=view*uv;

		h=atan2(fv.x(),-fv.z());
		p=asin(YsBound(-fv.y(),-1.0,1.0));
		b=0.0; // Tentative

		YsMatrix3x3 newView;
		newView.RotateXZ(h);
		newView.RotateZY(p);
		newView.RotateXY(b); // In fact it's no rotation.
		newView.MulInverse(uv,uv);
		b=atan2(-uv.x(),uv.y());
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


	for(auto &b : ball)
	{
		YsMatrix4x4 translation;
		translation.Translate(b.pos);

		YsMatrix4x4 fullMatrix=view*translation;

		GLfloat viewMat[16];
		fullMatrix.GetOpenGlCompatibleMatrix(viewMat);

		{
			GLfloat lightDir[]={0,0,1};

			YsGLSLShaded3DRenderer renderer;
			renderer.SetProjection(projMat);
			renderer.SetModelView(viewMat);
			renderer.SetLightDirectionInCameraCoordinate(0,lightDir);
			renderer.SetUniformColor(b.col);
			DrawPlainBall(renderer);
		}

		YsMatrix4x4 shadowMat;
		shadowMat.Scale(1.0,0.0,1.0);

		fullMatrix=view*shadowMat*translation;
		fullMatrix.GetOpenGlCompatibleMatrix(viewMat);
		{
			GLfloat color[]={0,0,0,1};

			YsGLSLPlain3DRenderer renderer;
			renderer.SetProjection(projMat);
			renderer.SetModelView(viewMat);
			renderer.SetUniformColor(color);
			DrawPlainBall(renderer);
		}
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
