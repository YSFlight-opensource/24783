#include <ysshellext.h>

#include <vector>

#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <fslazywindow.h>

#include "vertex_buffer_object.h"
#include "binary_stl.h"
#include "lattice.h"

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	enum STATE
	{
		STATE_NORMAL,
		STATE_DUAL,
		STATE_HIGHDHA
	};

	STATE state;
	std::vector <float> dualVtx;

	bool needRedraw;

	Ys3DDrawingEnvironment drawEnv;


	int prevMx,prevMy;


	YsShellExt shl;
	std::vector <float> vtx,nom;
	std::vector <float> col;
	YsVec3 min,max;

	std::vector <float> pickedVtx;

	std::vector <float> highDhaEdge;


	YsVec3 lastClick[2];



public:
	void LoadBinaryStl(const char fn[]);
	void CacheBoundingBox(void);

	void MouseCoordinateTo3DLine(YsVec3 ln[2],int mx,int my) const;
	YsShell::PolygonHandle PickedTriangle(int mx,int my) const;
	YsShell::VertexHandle PickedVertex(int mx,int my) const;

	static void GetNNeighbor(
	   std::vector <YsShell::PolygonHandle> &neiPlHd,
	   std::vector <int> &neiDist,
	   const YsShellExt &shl,YsShell::PolygonHandle plHd,int N);

	static std::vector <YsShell::VertexHandle> GetVertexWithinRadius(
	   YsShellExt &shl,YsShell::VertexHandle from,const double radius);

	static void GetBoundingBox(YsVec3 &min,YsVec3 &max,const std::vector <float> &vtx);
	static void PrepareLatticeForConnection(Lattice3d <std::vector <YsShell::VertexHandle> > &ltc,const std::vector <float> &vtx);
	static void VtxNomToYsShell(YsShellExt &shl,const std::vector <float> &vtx,const std::vector <float> &nom);
	static void YsShellToVtxNom(std::vector <float> &vtx,std::vector <float> &nom,std::vector <float> &col,const YsShellExt &shl);

	void MakeDual(void);
	void MakeHighDha(const double dhaThr);

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
	VtxNomToYsShell(shl,vtx,nom);
	shl.EnableSearch();
	CacheBoundingBox();
}

void FsLazyWindowApplication::CacheBoundingBox(void)
{
	shl.GetBoundingBox(min,max);
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

YsShell::PolygonHandle FsLazyWindowApplication::PickedTriangle(int mx,int my) const
{
	YsVec3 o,v;
	drawEnv.TransformScreenCoordTo3DLine(o,v,mx,my);

	YsShell::PolygonHandle picked=nullptr;
	double pickedDist=0.0;
	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		const YsVec3 tri[3]=
		{
			shl.GetVertexPosition(plVtHd[0]),
			shl.GetVertexPosition(plVtHd[1]),
			shl.GetVertexPosition(plVtHd[2]),
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
				if(0.0<dist && (picked==nullptr || dist<pickedDist))
				{
					picked=plHd;
					pickedDist=dist;
				}
			}
		}
	}

	return picked;
}

YsShell::VertexHandle FsLazyWindowApplication::PickedVertex(int mx,int my) const
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	double pickedZ=YsInfinity;
	YsShell::VertexHandle pickedVtHd=nullptr;
	for(auto vtHd : shl.AllVertex())
	{
		YsVec3 pos=shl.GetVertexPosition(vtHd);
		drawEnv.GetViewMatrix().Mul(pos,pos,1.0);
		drawEnv.GetProjectionMatrix().Mul(pos,pos,1.0);
		if(-1.0<=pos.z() && pos.z()<=1.0)
		{
			const double u=(pos.x()+1.0)/2.0;
			const double v=(pos.y()+1.0)/2.0;

			int x=(int)((double)wid*u);
			int y=hei-(int)((double)hei*v);
			if(mx-8<=x && x<=mx+8 && my-8<=y && y<=my+8)
			{
				if(nullptr==pickedVtHd || pos.z()<pickedZ)
				{
					pickedVtHd=vtHd;
					pickedZ=pos.z();
				}
			}
		}
	}

	return pickedVtHd;
}

/* static */ void FsLazyWindowApplication::GetNNeighbor(
	   std::vector <YsShell::PolygonHandle> &neiPlHd,
	   std::vector <int> &neiDist,
	   const YsShellExt &shl,YsShell::PolygonHandle plHd,int N)
{
	neiPlHd.clear();
	neiDist.clear();
	YsShellPolygonStore visited(shl.Conv());

	neiPlHd.push_back(plHd);
	neiDist.push_back(0);
	visited.Add(plHd);

	for(long long int ptr=0; ptr<neiPlHd.size(); ++ptr)
	{
		if(neiDist[ptr]<N)
		{
			YsShell::PolygonHandle plHd=neiPlHd[ptr];
			auto nEdge=shl.GetPolygonNumVertex(plHd);

			for(int edIdx=0; edIdx<nEdge; ++edIdx)
			{
				auto nei=shl.GetNeighborPolygon(plHd,edIdx);
				if(nullptr!=nei && YSTRUE!=visited.IsIncluded(nei))
				{
					neiPlHd.push_back(nei);
					neiDist.push_back(neiDist[ptr]+1);
					visited.Add(nei);
				}
			}
		}
	}
}

/* static */ std::vector <YsShell::VertexHandle> FsLazyWindowApplication::GetVertexWithinRadius(
	   YsShellExt &shl,YsShell::VertexHandle from,const double radius)
{
	std::vector <YsShell::VertexHandle> vtHd;
	const YsVec3 fromPos=shl.GetVertexPosition(from);
	YsShellVertexStore visited(shl.Conv());

	vtHd.push_back(from);
	visited.Add(from);
	for(long long int ptr=0; ptr<vtHd.size(); ++ptr)
	{
		for(auto connVtHd : shl.GetConnectedVertex(vtHd[ptr]))
		{
			if(YSTRUE!=visited.IsIncluded(connVtHd))
			{
				auto distSq=(shl.GetVertexPosition(connVtHd)-fromPos).GetSquareLength();
				if(distSq<radius*radius)
				{
					vtHd.push_back(connVtHd);
					visited.Add(connVtHd);
				}
			}
		}
	}

	return vtHd;
}

/* static */ void FsLazyWindowApplication::GetBoundingBox(YsVec3 &min,YsVec3 &max,const std::vector <float> &vtx)
{
	auto nVtx=vtx.size()/3;
	if(0==nVtx)
	{
		min=YsVec3::Origin();
		max=YsVec3::Origin();
	}
	else
	{
		YsBoundingBoxMaker3 mkBbx;
		for(decltype(nVtx) i=0; i<nVtx; ++i)
		{
			YsVec3 pos(vtx[i*3],vtx[i*3+1],vtx[i*3+2]);
			mkBbx.Add(pos);
		}
		mkBbx.Get(min,max);
	}
}
/* static */ void FsLazyWindowApplication::PrepareLatticeForConnection(Lattice3d <std::vector <YsShell::VertexHandle> > &ltc,const std::vector <float> &vtx)
{
	YsVec3 min,max;
	GetBoundingBox(min,max,vtx);
	double d=(max-min).GetLength()/100.0;
	min-=YsXYZ()*d; // Make absolutely sure that all vertices are inside.
	max+=YsXYZ()*d;

	auto nVtx=vtx.size()/3;
	ltc.Create(100,100,100);
	ltc.SetDimension(min,max);
}

/* static */ void FsLazyWindowApplication::VtxNomToYsShell(YsShellExt &shl,const std::vector <float> &vtx,const std::vector <float> &nom)
{
	Lattice3d <std::vector <YsShell::VertexHandle> > ltc;
	PrepareLatticeForConnection(ltc,vtx);

	shl.CleanUp();
	for(int i=0; i<vtx.size()/9; ++i)
	{
		const YsVec3 vtNom(nom[i*9],nom[i*9+1],nom[i*9+2]);
		const YsVec3 vtPos[3]=
		{
			YsVec3(vtx[i*9  ],vtx[i*9+1],vtx[i*9+2]),
			YsVec3(vtx[i*9+3],vtx[i*9+4],vtx[i*9+5]),
			YsVec3(vtx[i*9+6],vtx[i*9+7],vtx[i*9+8]),
		};
		YsShell::VertexHandle vtHd[3];
		for(int i=0; i<3; ++i)
		{
			vtHd[i]=nullptr;
			auto idx=ltc.GetBlockIndex(vtPos[i]);
			if(true==ltc.IsInRange(idx))
			{
				for(auto tstVtHd : ltc.Elem(idx.x(),idx.y(),idx.z()))
				{
					if(shl.GetVertexPosition(tstVtHd)==vtPos[i])
					{
						vtHd[i]=tstVtHd;
						break;
					}
				}
			}
			if(nullptr==vtHd[i])
			{
				vtHd[i]=shl.AddVertex(vtPos[i]);
				if(true==ltc.IsInRange(idx))
				{
					ltc.Elem(idx.x(),idx.y(),idx.z()).push_back(vtHd[i]);
				}
			}
		}
		YsShell::PolygonHandle plHd;
		plHd=shl.AddPolygon(3,vtHd);
		shl.SetPolygonNormal(plHd,vtNom);
		shl.SetPolygonColor(plHd,YsBlue());
	}
}
/* static */ void FsLazyWindowApplication::YsShellToVtxNom(std::vector <float> &vtx,std::vector <float> &nom,std::vector <float> &col,const YsShellExt &shl)
{
	vtx.clear();
	nom.clear();
	col.clear();
	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		if(3<=plVtHd.GetN())
		{
			auto plNom=shl.GetNormal(plHd);
			auto plCol=shl.GetColor(plHd);
			for(auto vtHd : plVtHd)
			{
				auto vtPos=shl.GetVertexPosition(vtHd);
				vtx.push_back(vtPos.xf());
				vtx.push_back(vtPos.yf());
				vtx.push_back(vtPos.zf());
				nom.push_back(plNom.xf());
				nom.push_back(plNom.yf());
				nom.push_back(plNom.zf());
				col.push_back(plCol.Rf());
				col.push_back(plCol.Gf());
				col.push_back(plCol.Bf());
				col.push_back(1);
			}
		}
	}
}

void FsLazyWindowApplication::MakeDual(void)
{
	dualVtx.clear();
	for(auto plHd : shl.AllPolygon())
	{
		auto cen=shl.GetCenter(plHd);
		auto nEdge=shl.GetPolygonNumVertex(plHd);
		for(decltype(nEdge) edIdx=0; edIdx<nEdge; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd && shl.GetSearchKey(plHd)<shl.GetSearchKey(neiPlHd))
			{
				auto neiCen=shl.GetCenter(neiPlHd);
				dualVtx.push_back(cen.xf());
				dualVtx.push_back(cen.yf());
				dualVtx.push_back(cen.zf());
				dualVtx.push_back(neiCen.xf());
				dualVtx.push_back(neiCen.yf());
				dualVtx.push_back(neiCen.zf());
			}
		}
	}
}

void FsLazyWindowApplication::MakeHighDha(const double dhaThr)
{
	highDhaEdge.clear();
	for(auto plHd : shl.AllPolygon())
	{
		auto nEdge=shl.GetPolygonNumVertex(plHd);
		auto plVtHd=shl.GetPolygonVertex(plHd);
		auto nom0=shl.GetNormal(plHd);
		for(decltype(nEdge) edIdx=0; edIdx<nEdge; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd && shl.GetSearchKey(plHd)<shl.GetSearchKey(neiPlHd))
			{
				auto nom1=shl.GetNormal(neiPlHd);
				if(nom0*nom1<cos(dhaThr))
				{
					YsVec3 edVtPos[2]=
					{
						shl.GetVertexPosition(plVtHd[edIdx]),
						shl.GetVertexPosition(plVtHd.GetCyclic(edIdx+1))
					};
					highDhaEdge.push_back(edVtPos[0].xf());
					highDhaEdge.push_back(edVtPos[0].yf());
					highDhaEdge.push_back(edVtPos[0].zf());
					highDhaEdge.push_back(edVtPos[1].xf());
					highDhaEdge.push_back(edVtPos[1].yf());
					highDhaEdge.push_back(edVtPos[1].zf());
				}
			}
		}
	}
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

	state=STATE_NORMAL;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
	if(2<=argc)
	{
		if(true==IsAsciiStl(argv[1]))
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
		drawEnv.SetWindowSize(wid,hei);
		drawEnv.SetViewportByTwoCorner(0,0,wid,hei);
		drawEnv.TransformScreenCoordTo3DLine(lastClick[0],lastClick[1],mx,my);
		lastClick[1]*=80.0;
		lastClick[1]+=lastClick[0];

		auto plHd=PickedTriangle(mx,my);
		if(nullptr!=plHd)
		{
			std::vector <YsShell::PolygonHandle> neiPlHd;
			std::vector <int> neiDist;
			GetNNeighbor(neiPlHd,neiDist,shl,plHd,3);
			for(int i=0; i<neiPlHd.size(); ++i)
			{
				switch(neiDist[i])
				{
				case 0:
					shl.SetPolygonColor(neiPlHd[i],YsBlue());
					break;
				case 1:
					shl.SetPolygonColor(neiPlHd[i],YsRed());
					break;
				case 2:
					shl.SetPolygonColor(neiPlHd[i],YsCyan());
					break;
				}
			}
		}
		YsShellToVtxNom(vtx,nom,col,shl);
	}
	if(evt==FSMOUSEEVENT_RBUTTONDOWN)
	{
		auto vtHd0=PickedVertex(mx,my);
		if(nullptr!=vtHd0)
		{
			pickedVtx.clear();
			auto allVtHd=GetVertexWithinRadius(shl,vtHd0,2.0);
			for(auto vtHd : allVtHd)
			{
				auto pos=shl.GetVertexPosition(vtHd);
				pickedVtx.push_back(pos.xf());
				pickedVtx.push_back(pos.yf());
				pickedVtx.push_back(pos.zf());
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
	if(FSKEY_D==key)
	{
		MakeDual();
		state=STATE_DUAL;
	}
	if(FSKEY_H==key)
	{
		MakeHighDha(YsPi/4.0); // 45 deg
		state=STATE_HIGHDHA;
	}
	if(FSKEY_N==key)
	{
		state=STATE_NORMAL;
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
	drawEnv.SetViewDistance((max-min).GetLength()/2.0);

	GLfloat projMat[16];
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);


	auto &view=drawEnv.GetViewMatrix();

	YsMatrix4x4 modeling;

	YsMatrix4x4 fullMatrix=view*modeling;

	GLfloat viewMat[16];
	fullMatrix.GetOpenGlCompatibleMatrix(viewMat);

	if(STATE_NORMAL==state)
	{
		GLfloat lightDir[]={0,0,1};

		YsGLSLShaded3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);
		renderer.SetLightDirectionInCameraCoordinate(0,lightDir);

		renderer.DrawVtxNomCol(GL_TRIANGLES,vtx.size()/3,vtx.data(),nom.data(),col.data());
	}
	else if(STATE_DUAL==state)
	{
		YsGLSLPlain3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);

		GLfloat color[4]={0,0,0,1};
		renderer.SetUniformColor(color);
		renderer.DrawVtx(GL_LINES,dualVtx.size()/3,dualVtx.data());
	}
	else if(STATE_HIGHDHA==state)
	{
		YsGLSLPlain3DRenderer renderer;  // Again, do not nest the renderer!
		renderer.SetProjection(projMat);
		renderer.SetModelView(viewMat);

		GLfloat color[4]={0,0,0,1};
		renderer.SetUniformColor(color);
		renderer.DrawVtx(GL_LINES,highDhaEdge.size()/3,highDhaEdge.data());
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

		glPointSize(4);
		GLfloat red[4]={1,0,0,1};
		renderer.SetUniformColor(red);
		renderer.DrawVtx(GL_POINTS,pickedVtx.size()/3,pickedVtx.data());
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
