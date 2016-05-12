#ifndef LATTICE_IS_INCLUDED
#define LATTICE_IS_INCLUDED
/* { */

template <class T>
class Lattice3d
{
protected:
	int nx,ny,nz;
	T *dat;

private:
	Lattice3d(const Lattice3d <T> &incoming);
	Lattice3d &operator=(const Lattice3d <T> &incoming);
public:
	Lattice3d();
	~Lattice3d();
	void CleanUp(void);

	T &Elem(int x,int y,int z);
	const T &Elem(int x,int y,int z) const;

	/*! nx,ny,nz=number of blocks, not nodes. */
	void Create(int nx,int ny,int nz);
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

/* } */
#endif
