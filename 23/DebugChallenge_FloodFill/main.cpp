#include <vector>

#include <ysglslcpp.h>
#include <fslazywindow.h>

// Question: Find a bug in FloodFill and tell when this FloodFill fails.

class Vec2
{
public:
	int x,y;
};

void FloodFill(char ltc[],int ltcWid,int ltcHei,int x0,int y0,char from,char to)
{
	if(x0<0 || ltcWid<=x0 || y0<0 || ltcHei<=y0)
	{
		return;
	}
	if(ltc[y0*ltcWid+x0]!=from)
	{
		return;
	}

	std::vector <Vec2> todo;
	Vec2 vec2;
	vec2.x=x0;
	vec2.y=y0;
	todo.push_back(vec2);
	while(0<todo.size())
	{
		auto pos=todo.back();
		todo.pop_back();

		Vec2 nei[4];
		nei[0].x=pos.x-1;
		nei[0].y=pos.y;
		nei[1].x=pos.x+1;
		nei[1].y=pos.y;
		nei[2].x=pos.x;
		nei[2].y=pos.y-1;
		nei[3].x=pos.x;
		nei[3].y=pos.y+1;
		for(auto n : nei)
		{
			if(0<=n.x && n.x<ltcWid && 0<=n.y && n.y<ltcHei &&
			   ltc[n.y*ltcWid+n.x]==from)
			{
				ltc[n.y*ltcWid+n.x]=to;
				todo.push_back(n);
			}
		}
	}
}



class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	enum
	{
		SCALE=16
	};

	int ltcWid,ltcHei;
	std::vector <char> lattice;

	std::vector <float> vtxBuf,colBuf;
	void RemakeVertexAttribArray(void);

	std::vector <float> lineVtxBuf;

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

void FsLazyWindowApplication::RemakeVertexAttribArray(void)
{
	lineVtxBuf.clear();
	vtxBuf.clear();
	colBuf.clear();
	for(int y=0; y<ltcHei; ++y)
	{
		for(int x=0; x<ltcWid; ++x)
		{
			GLfloat col[4]={1,1,1,1};

			switch(lattice[y*ltcWid+x])
			{
			default:
			case 0:
				break;
			case 1:
				col[0]=0;
				col[1]=0;
				col[2]=1;
				col[3]=1;
				break;
			}

			float xf=(float)x*(float)SCALE;
			float yf=(float)y*(float)SCALE;
			vtxBuf.push_back(xf);
			vtxBuf.push_back(yf);

			vtxBuf.push_back(xf);
			vtxBuf.push_back(yf+(float)SCALE);

			vtxBuf.push_back(xf+(float)SCALE);
			vtxBuf.push_back(yf+(float)SCALE);

			vtxBuf.push_back(xf+(float)SCALE);
			vtxBuf.push_back(yf+(float)SCALE);

			vtxBuf.push_back(xf+(float)SCALE);
			vtxBuf.push_back(yf);

			vtxBuf.push_back(xf);
			vtxBuf.push_back(yf);

			for(int repeat=0; repeat<6; ++repeat)
			{
				colBuf.push_back(col[0]);
				colBuf.push_back(col[1]);
				colBuf.push_back(col[2]);
				colBuf.push_back(col[3]);
			}
		}
	}

	for(int y=0; y<=ltcHei; ++y)
	{
		lineVtxBuf.push_back(0);
		lineVtxBuf.push_back((float)y*(float)SCALE);

		lineVtxBuf.push_back((float)ltcWid*(float)SCALE);
		lineVtxBuf.push_back((float)y*(float)SCALE);
	}
	for(int x=0; x<=ltcWid; ++x)
	{
		lineVtxBuf.push_back((float)x*(float)SCALE);
		lineVtxBuf.push_back(0);

		lineVtxBuf.push_back((float)x*(float)SCALE);
		lineVtxBuf.push_back((float)ltcHei*(float)SCALE);
	}
}


FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;

	ltcWid=32;
	ltcHei=32;
	lattice.resize(ltcWid*ltcHei);
	for(auto &c : lattice)
	{
		c=0;
	}

	RemakeVertexAttribArray();
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
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}


	int lb,mb,rb,mx,my;
	auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(0!=lb)
	{
		int lx=mx/SCALE;
		int ly=my/SCALE;
		if(0<=lx && lx<ltcWid && 0<=ly && ly<ltcHei)
		{
			lattice[ly*ltcWid+lx]=1;
			RemakeVertexAttribArray();
		}
	}

	if(FSKEY_F==key)
	{
		int lx=mx/SCALE;
		int ly=my/SCALE;
		FloodFill(lattice.data(),ltcWid,ltcHei,lx,ly,0,1);
		RemakeVertexAttribArray();
	}


	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	{
		YsGLSL2DRenderer renderer;
		renderer.UseWindowCoordinateTopLeftAsOrigin();
		renderer.DrawVtxCol(GL_TRIANGLES,vtxBuf.size()/2,vtxBuf.data(),colBuf.data());

		GLfloat black[4]={0,0,0,1};
		renderer.SetUniformColor(black);
		renderer.DrawVtx(GL_LINES,lineVtxBuf.size()/2,lineVtxBuf.data());
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
