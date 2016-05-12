#ifndef LATTICE_IS_INCLUDED
#define LATTICE_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <vector>
#include <math.h>

template <class T>
class Lattice3d
{
protected:
	int nx,ny,nz;
	T *dat;
	YsVec3 min,max,dgn;

private:
	Lattice3d(const Lattice3d <T> &incoming);
	Lattice3d &operator=(const Lattice3d <T> &incoming);
public:
	Lattice3d();
	~Lattice3d();
	void CleanUp(void);

	int GetN(void) const;
	T &Elem(int i);
	const T &Elem(int i) const;

	T &Elem(int x,int y,int z);
	const T &Elem(int x,int y,int z) const;

	/*! nx,ny,nz=number of blocks, not nodes. */
	void Create(int nx,int ny,int nz);

	void SetDimension(const YsVec3 &min,const YsVec3 &max);
	YsVec3i GetBlockIndex(const YsVec3 &pos) const;
	bool IsInRange(YsVec3i idx) const;

	void GetBlockRange(YsVec3 &min,YsVec3 &max,YsVec3i idx) const;

	bool ClipLine(YsVec3 &p1,YsVec3 &p2,const YsVec3 o,const YsVec3 n,const YsVec3 &min,const YsVec3 &max) const;
	bool ClipLine(YsVec3 &p1,YsVec3 &p2,const YsVec3 o,const YsVec3 n) const;
	std::vector <YsVec3i> GetIntersectingBlock(const YsVec3 o,const YsVec3 v) const;
};

template <class T>
Lattice3d<T>::Lattice3d()
{
	nx=0;
	ny=0;
	nz=0;
	dat=nullptr;
}

template <class T>
Lattice3d<T>::~Lattice3d()
{
	CleanUp();
}

template <class T>
void Lattice3d<T>::CleanUp(void)
{
	if(nullptr!=dat)
	{
		delete [] dat;
	}
	nx=0;
	ny=0;
	nz=0;
	dat=nullptr;
}

template <class T>
int Lattice3d<T>::GetN(void) const
{
	return (nx+1)*(ny+1)*(nz+1);
}
template <class T>
T &Lattice3d<T>::Elem(int i)
{
	return dat[i];
}
template <class T>
const T &Lattice3d<T>::Elem(int i) const
{
	return dat[i];
}

template <class T>
void Lattice3d<T>::Create(int nx,int ny,int nz)
{
	CleanUp();
	if(0<nx && 0<ny && 0<nz)
	{
		this->nx=nx;
		this->ny=ny;
		this->nz=nz;
		dat=new T [(nx+1)*(ny+1)*(nz+1)];
	}
}

template <class T>
T &Lattice3d<T>::Elem(int x,int y,int z)
{
	return dat[z*(nx+1)*(ny+1)+y*(nx+1)+x];
}

template <class T>
const T &Lattice3d<T>::Elem(int x,int y,int z) const
{
	return dat[z*(nx+1)*(ny+1)+y*(nx+1)+x];
}

template <class T>
void Lattice3d<T>::SetDimension(const YsVec3 &min,const YsVec3 &max)
{
	this->min=min;
	this->max=max;
	this->dgn=max-min;
}

template <class T>
YsVec3i Lattice3d<T>::GetBlockIndex(const YsVec3 &pos) const
{
	const YsVec3 rel=pos-min;
	const double tx=rel.x()/dgn.x();
	const double ty=rel.y()/dgn.y();
	const double tz=rel.z()/dgn.z();
	int ix=(int)(tx*(double)nx);
	int iy=(int)(ty*(double)ny);
	int iz=(int)(tz*(double)nz);
	return YsVec3i(ix,iy,iz);
}

template <class T>
void Lattice3d<T>::GetBlockRange(YsVec3 &min,YsVec3 &max,YsVec3i idx) const
{
	const double dx=dgn.x()/(double)nx;
	const double dy=dgn.y()/(double)ny;
	const double dz=dgn.z()/(double)nz;
	min.SetX(this->min.x()+dx*(double)idx.x());
	min.SetY(this->min.y()+dy*(double)idx.y());
	min.SetZ(this->min.z()+dz*(double)idx.z());
	max=min;
	max.AddX(dx);
	max.AddY(dy);
	max.AddZ(dz);
}

template <class T>
bool Lattice3d<T>::IsInRange(YsVec3i idx) const
{
	if(0<=idx.x() && idx.x()<=nx &&
	   0<=idx.y() && idx.y()<=ny &&
	   0<=idx.z() && idx.z()<=nz)
	{
		return true;
	}
	return false;
}

template <class T>
bool Lattice3d<T>::ClipLine(YsVec3 &p0,YsVec3 &p1,const YsVec3 o,const YsVec3 v,const YsVec3 &min,const YsVec3 &max) const
{
	// YsTolerance is a small number
	// YsInfinity is a very large number.
	double tmin=-YsInfinity,tmax=YsInfinity;

	for(int dim=0; dim<3; ++dim)
	{
		if(YsTolerance<fabs(v[dim]))
		{
			double t0=(min[dim]-o[dim])/v[dim];
			double t1=(max[dim]-o[dim])/v[dim];
			YsVec3 i0=o+t0*v;
			YsVec3 i1=o+t1*v;
			// Reduce adverse effect from numerical error.
			// minx may not be equal to 
			// ox+vx*((minx-ox)/vx)
			i0[dim]=min[dim];
			i1[dim]=max[dim];

			if(t0>t1)
			{
				std::swap(t0,t1);
				std::swap(i0,i1);
			}

			if(tmin<t0)
			{
				tmin=t0;
				p0=i0;
			}
			if(t1<tmax)
			{
				tmax=t1;
				p1=i1;
			}
		}
	}

	if(tmax<tmin)
	{
		return false;
	}

	for(int dim=0; dim<3; ++dim)
	{
		if((p0[dim]<min[dim] && p1[dim]<min[dim]) ||
		   (p0[dim]>max[dim] && p1[dim]>max[dim]))
		{
			return false;
		}
	}

	return true;
}

template <class T>
bool Lattice3d<T>::ClipLine(YsVec3 &p1,YsVec3 &p2,const YsVec3 o,const YsVec3 n) const
{
	return ClipLine(p1,p2,o,n,min,max);
}

template <class T>
std::vector <YsVec3i> Lattice3d<T>::GetIntersectingBlock(const YsVec3 o,const YsVec3 v) const
{
	std::vector <YsVec3i> itscIdx;
	YsVec3 p0,p1;
	if(true==ClipLine(p0,p1,o,v))
	{
		YsVec3i i0=GetBlockIndex(p0);
		YsVec3i i1=GetBlockIndex(p1);
		YsVec3i iMin,iMax;
		iMin.Set(
		    YsSmaller(i0.x(),i1.x()),
		    YsSmaller(i0.y(),i1.y()),
		    YsSmaller(i0.z(),i1.z()));
		iMax.Set(
		    YsGreater(i0.x(),i1.x()),
		    YsGreater(i0.y(),i1.y()),
		    YsGreater(i0.z(),i1.z()));
		for(auto i : YsVec3iRange(iMin,iMax))
		{
			YsVec3 min,max,t0,t1;
			GetBlockRange(min,max,i);
			if(true==ClipLine(t0,t1,o,v,min,max))
			{
				itscIdx.push_back(i);
			}
		}
	}
	return itscIdx;
}

/* } */
#endif
