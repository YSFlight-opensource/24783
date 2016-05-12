#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "fssimplewindow.h"

#include "ysglfontdata.h"


template <class T,const int N>
class VecN
{
protected:
	T v[N];
public:
	T Get(int i) const;
	void Set(int i,const T &incoming);
};

template <class T,const int N>
T VecN<T,N>::Get(int i) const
{
	return v[i-1];
}

template <class T,const int N>
void VecN<T,N>::Set(int i,const T &incoming)
{
	v[i-1]=incoming;
}


class Vec2d : public VecN<double,2>
{
public:
	static Vec2d Origin(void)
	{
		Vec2d v;
		v.SetX(0.0);
		v.SetY(0.0);
		return v;
	}

	const double x(void) const
	{
		return v[0];
	}
	const double y(void) const
	{
		return v[1];
	}
	void SetX(const double x)
	{
		v[0]=x;
	}
	void SetY(const double y)
	{
		v[1]=y;
	}
	void AddX(const double x)
	{
		v[0]+=x;
	}
	void AddY(const double y)
	{
		v[1]+=y;
	}
	void MulX(const double x)
	{
		v[0]*=x;
	}
	void MulY(const double y)
	{
		v[1]*=y;
	}
	void DivX(const double x)
	{
		v[0]/=x;
	}
	void DivY(const double y)
	{
		v[1]/=y;
	}
};

Vec2d operator+(Vec2d v1,Vec2d v2)
{
	v1.AddX(v2.x());
	v1.AddY(v2.y());
	return v1;
}
Vec2d operator/(Vec2d v,const double s)
{
	v.DivX(s);
	v.DivY(s);
	return v;
}
Vec2d operator*(Vec2d v,const double s)
{
	v.MulX(s);
	v.MulY(s);
	return v;
}
Vec2d operator*(const double s,Vec2d v)
{
	v.MulX(s);
	v.MulY(s);
	return v;
}



class Ball
{
protected:
	double m;
	Vec2d pos,vel;

public:
	void Initialize(void);
	void Move(const double dt);
	void CalculateCollision(Ball &withThisBall);
	void Bounce(void);
	void Draw(void) const;
};

void DrawCircle(int cx,int cy,int rad,int fill)
{
    const double YS_PI=3.1415927;

    if(0!=fill)
    {
        glBegin(GL_POLYGON);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    int i;
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=(double)cx+cos(angle)*(double)rad;
        double y=(double)cy+sin(angle)*(double)rad;
        glVertex2d(x,y);
    }

    glEnd();
}

void Ball::Initialize(void)
{
	m=1.0;

	// pos=Vec2d::Origin();

	pos.SetX((double)(rand()%80-40));  // -40 to  39
	pos.SetY((double)(rand()%40+10));  //  10 to  49

	vel.SetX((double)(rand()%40-20)); // -20 to +19
	vel.SetY(0.0);
}

void Ball::Move(const double dt)
{
    double fx,fy,ax,ay;

	pos=pos+vel*dt;

    fx=0.0;
    fy=-m*9.8;  // Gravity

	Vec2d a;
	a.SetX(fx);
	a.SetY(fy);
    a=a/m;

	vel=vel+a*dt;
}

void Ball::Bounce(void)
{
    if(pos.y()<1.0 && vel.y()<0.0)
    {
        vel.MulY(-1.0);
    }

    if(pos.x()<-39.0 && vel.x()<0.0)
    {
        vel.MulX(-1.0);
    }
    else if(pos.x()>39.0 && vel.x()>0.0)
    {
        vel.MulX(-1.0);
    }
}

void Ball::CalculateCollision(Ball &withThisBall)
{
	const double x1=this->pos.x();
	const double y1=this->pos.y();
	const double x2=withThisBall.pos.x();
	const double y2=withThisBall.pos.y();
	double vx1=this->vel.x();
	double vy1=this->vel.y();
	double vx2=withThisBall.vel.x();
	double vy2=withThisBall.vel.y();

	const double radius=1.0;

    double dx=x2-x1,dy=y2-y1;
    double d=dx*dx+dy*dy;
    if(d<(radius*2.0)*(radius*2.0))
    {
        double nx,ny,nl;
        nx=x2-x1;
        ny=y2-y1;

        nl=nx*nx+ny*ny;
        if(0.0<nl)
        {
            nl=sqrt(nl);
            nx/=nl;
            ny/=nl;

            double nv1,nv2;
            nv1=(vx1*nx+vy1*ny);
            nv2=(vx2*nx+vy2*ny);

            if((nv2>0.0 && nv2<nv1) || (nv1<0.0 && nv2<nv1) || (nv1>0.0 && nv2<0.0)) // Means closing in.  Otherwise, two molecules are moving away.
            {
                vx1=vx1-nx*nv1+nx*nv2;
                vy1=vy1-ny*nv1+ny*nv2;

                vx2=vx2-nx*nv2+nx*nv1;
                vy2=vy2-ny*nv2+ny*nv1;
            }
        }
    }

	this->vel.SetX(vx1);
	this->vel.SetY(vy1);
	withThisBall.vel.SetX(vx2);
	withThisBall.vel.SetY(vy2);
}

void CollisionDetection(int n,Ball ball[]) // <- This will be written a lot cleaner when you learn class/structure
{
    // Assuming all ball has the same radius and same mass
    int i,j;
    for(i=0; i<n; i++)
    {
        for(j=i+1; j<n; j++)
        {
			ball[i].CalculateCollision(ball[j]);
        }
    }
}

void Ball::Draw(void) const
{
    DrawCircle(400+(pos.x()*10.0),600-pos.y()*10.0,10,1); // 10pixel=1.0m
}


const int nBall=10;
Ball ball[nBall];

int main(void)
{
    int i;

    srand(time(NULL));

    for(i=0; i<nBall; i++)
    {
		ball[i].Initialize();
    }

    FsOpenWindow(16,16,800,600,1);

	FsPassedTime();

    FsPollDevice();
    while(FSKEY_NULL==FsInkey())
    {
        FsPollDevice();

		int passedInMs=FsPassedTime();
		double dt=(double)passedInMs/1000.0;

        for(i=0; i<nBall; i++)
        {
			ball[i].Move(dt);
            ball[i].Bounce();
        }
        CollisionDetection(nBall,ball);

        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glColor3ub(0,255,0);
        for(i=0; i<nBall; i++)
        {
			ball[i].Draw();
        }

		glRasterPos2i(40,40);
		char str[256];
		sprintf(str,"%lf",dt);
		YsGlDrawFontBitmap16x24(str);

        FsSwapBuffers();

        FsSleep(10);
    }
}

