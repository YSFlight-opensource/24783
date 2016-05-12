#ifndef VLARRAY_IS_INCLUDED
#define VLARRAY_IS_INCLUDED
/* { */

template <class T>
class VLArray
{
private:
	long long int len;
	long long int nAvailable;
	T *dat;
public:
	VLArray();
	~VLArray();
	void CleanUp(void);
	long long int GetN(void) const;

	VLArray(const VLArray <T> &incoming);
	VLArray <T> &operator=(const VLArray <T> &incoming);
	void CopyFrom(const VLArray <T> &incoming);

	VLArray(VLArray <T> &&incoming);
	VLArray <T> &operator=(VLArray <T> &&incoming);
	void Swap(VLArray <T> &incoming);

	void Resize(long long int newSize);
	void Grow(void);
	void Add(const T &newElem);

	T &operator[](long long int idx);
	const T &operator[](long long int idx) const;
};
template <class T>
VLArray<T>::VLArray()
{
	len=0;
	nAvailable=0;
	dat=nullptr;
}
template <class T>
VLArray<T>::~VLArray()
{
	CleanUp();
}
template <class T>
void VLArray<T>::CleanUp(void)
{
	if(nullptr!=dat)
	{
		delete [] dat;
		dat=nullptr;
	}
	len=0;
	nAvailable=0;
}
template <class T>
long long int VLArray<T>::GetN(void) const
{
	return len;
}
template <class T>
VLArray<T>::VLArray(const VLArray &incoming)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	dat=nullptr;
	CopyFrom(incoming);
}
template <class T>
VLArray <T> &VLArray<T>::operator=(const VLArray &incoming)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	CopyFrom(incoming);
	return *this;
}
template <class T>
void VLArray<T>::CopyFrom(const VLArray &incoming)
{
	if(this!=&incoming)
	{
		CleanUp();
		if(0<incoming.len)
		{
			this->len=incoming.len;
			this->nAvailable=incoming.nAvailable;
			this->dat=new T [incoming.nAvailable];
			for(decltype(len) i=0; i<incoming.len; ++i)
			{
				this->dat[i]=incoming.dat[i];
			}
		}
	}
}

template <class T>
VLArray<T>::VLArray(VLArray <T> &&incoming)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	dat=nullptr;
	Swap(incoming);
}
template <class T>
VLArray <T> &VLArray<T>::operator=(VLArray <T> &&incoming)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	Swap(incoming);
	return *this;
}

template <class T>
void VLArray<T>::Swap(VLArray <T> &incoming)
{
	if(&incoming!=this)
	{
		auto dat=incoming.dat;
		auto len=incoming.len;
		auto nAvailable=incoming.nAvailable;

		incoming.dat=this->dat;
		incoming.len=this->len;
		incoming.nAvailable=this->nAvailable;

		this->dat=dat;
		this->len=len;
		this->nAvailable=nAvailable;
	}
}

template <class T>
void VLArray<T>::Resize(long long int newSize)
{
	if(0>=newSize)
	{
		CleanUp();
	}
	else
	{
		long long int newNAvailable=1;
		while(newNAvailable<newSize)
		{
			newNAvailable*=2;
		}

		T *newDat=new T [newNAvailable];
		for(decltype(len) i=0; i<len && i<newSize; ++i)
		{
			newDat[i]=dat[i];
		}
		if(nullptr!=dat)
		{
			delete [] dat;
		}
		dat=newDat;
		len=newSize;
		nAvailable=newNAvailable;
	}
}

template <class T>
void VLArray<T>::Grow(void)
{
	if(0==nAvailable) // Mean empty
	{
		dat=new T [1];
		nAvailable=1;
	}
	else
	{
		nAvailable*=2;
		T *newDat=new T[nAvailable];
		for(decltype(len) i=0; i<len; ++i)
		{
			newDat[i]=dat[i];
		}
		if(nullptr!=dat)
		{
			delete [] dat;
		}
		dat=newDat;
	}
}

template <class T>
void VLArray<T>::Add(const T &newElem)
{
	if(nAvailable<=len)
	{
		Grow();
	}
	dat[len]=newElem;
	++len;
}
template <class T>
T &VLArray<T>::operator[](long long int idx)
{
	return dat[idx];
}
template <class T>
const T &VLArray<T>::operator[](long long int idx) const
{
	return dat[idx];
}

/* } */
#endif
