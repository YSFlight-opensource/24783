#include <fslazywindow.h>
#include <ysglfontdata.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bintree.h"


#include <vector>


void MakeLinear(std::vector <int> &v,const BinaryTree <int,int>::Node *node)
{
	if(nullptr!=node)
	{
		MakeLinear(v,node->left);
		v.push_back(node->key);
		MakeLinear(v,node->right);
	}
}

void BreadthFirstTraversal(const BinaryTree <int,int>::Node *rootNode)
{
	std::vector <const BinaryTree <int,int>::Node *> todo;
	if(nullptr!=rootNode)
	{
		todo.push_back(rootNode);

		long long int firstPtr=0;
		while(firstPtr<todo.size())
		{
			auto node=todo[firstPtr];
			printf("%d ",node->key);

			if(nullptr!=node->left)
			{
				todo.push_back(node->left);
			}
			if(nullptr!=node->right)
			{
				todo.push_back(node->right);
			}

			++firstPtr;
		}
	}
}



class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	BinaryTree <int,int> btree;
	const BinaryTree <int,int>::Node *mouseOn;

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

	void Draw(const BinaryTree <int,int>::Node *node,int x0,int x1,int y,int yStep) const;

	void VerifyTree(void) const;

public:
	const BinaryTree <int,int>::Node *PickedNode(int mx,int my) const;
private:
	const BinaryTree <int,int>::Node *PickedNode(
	    const BinaryTree <int,int>::Node *node,
	    int mx,int my,int x0,int x1,int y,int yStep) const;

};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
	mouseOn=nullptr;
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
	srand((int)time(nullptr));
	for(int i=0; i<50; ++i)
	{
		btree.Add(rand()%100,0);
	}
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	int lb,mb,rb,mx,my;
	FsGetMouseEvent(lb,mb,rb,mx,my);
	mouseOn=PickedNode(mx,my);

	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	if(FSKEY_D==key && nullptr!=mouseOn)
	{
		btree.Delete(mouseOn);
		VerifyTree();
	}
	if(FSKEY_L==key && nullptr!=mouseOn)
	{
		btree.RotateLeft(mouseOn);
		VerifyTree();
	}
	if(FSKEY_B==key)
	{
		BreadthFirstTraversal(btree.GetRoot());
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Draw(btree.GetRoot(),0,wid,40,40);
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

void FsLazyWindowApplication::Draw(const BinaryTree <int,int>::Node *node,int x0,int x1,int y,int yStep) const
{
	if(nullptr!=node)
	{
		int x=(x0+x1)/2;

		glColor3ub(0,255,0);
		glBegin(GL_LINES);
		if(nullptr!=node->left)
		{
			int xNext=(x0+x)/2;
			int yNext=y+yStep;
			glVertex2i(x,y);
			glVertex2i(xNext,yNext);
		}
		if(nullptr!=node->right)
		{
			int xNext=(x+x1)/2;
			int yNext=y+yStep;
			glVertex2i(x,y);
			glVertex2i(xNext,yNext);
		}
		glEnd();

		if(node==mouseOn)
		{
			glColor3ub(255,0,0);
		}
		else 
		{
			glColor3ub(0,0,0);
		}
		glRasterPos2d(x-15,y);

		char str[16];
		sprintf(str,"%d",node->key);
		YsGlDrawFontBitmap10x14(str);

		Draw(node->left,x0,x,y+yStep,yStep);
		Draw(node->right,x,x1,y+yStep,yStep);
	}
}

const BinaryTree <int,int>::Node *FsLazyWindowApplication::PickedNode(int mx,int my) const
{
	int wid,hei;
	FsGetWindowSize(wid,hei);
	return PickedNode(btree.GetRoot(),mx,my,0,wid,40,40);
}

const BinaryTree <int,int>::Node *FsLazyWindowApplication::PickedNode(
	    const BinaryTree <int,int>::Node *node,
	    int mx,int my,int x0,int x1,int y,int yStep) const
{
	if(nullptr!=node)
	{
		int x=(x0+x1)/2;
		if(x-15<=mx && mx<=x+15 && y-14<=my && my<=y)
		{
			return node;
		}

		auto picked=PickedNode(node->left,mx,my,x0,x,y+yStep,yStep);
		if(nullptr!=picked)
		{
			return picked;
		}
		picked=PickedNode(node->right,mx,my,x,x1,y+yStep,yStep);
		if(nullptr!=picked)
		{
			return picked;
		}
	}
	return nullptr;
}

void FsLazyWindowApplication::VerifyTree(void) const
{
	std::vector <int> v;
	MakeLinear(v,btree.GetRoot());
	for(int i=0; i<v.size(); ++i)
	{
		printf(" %d",v[i]);
	}
	printf("\n");
	for(int i=0; i+1<v.size(); ++i)
	{
		if(v[i]>v[i+1])
		{
			printf("Error! %d>%d\n",v[i],v[i+1]);
		}
	}
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
