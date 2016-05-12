#include <stdio.h>
#include <stdlib.h>
#include <time.h>

template <class T>
void Shuffle(int n,T &dat)
{
	for(int i=0; i<n; ++i)
	{
		auto r=rand()%n;
		auto c=dat[i];
		dat[i]=dat[r];
		dat[r]=c;
	}
} 


template <class T>
class Array
{
protected:
	long long int len;
	T *dat;
public:
	Array(const Array &incoming)
	{
		len=0;
		dat=nullptr;
		CopyFrom(incoming);
	}
	Array &operator=(const Array &incoming)
	{
		CopyFrom(incoming);
		return *this;
	}
	void CleanUp(void)
	{
		if(nullptr!=dat)
		{
			delete [] dat;
			len=0;
			dat=nullptr;
		}
	}
	void CopyFrom(const Array &incoming)
	{
		if(&incoming!=this)
		{
			CleanUp();
			dat=new T [incoming.len];
			for(long long int i=0; i<incoming.len; ++i)
			{
				dat[i]=incoming.dat[i];
			}
			len=incoming.len;
		}
	}

	Array()
	{
		len=0;
		dat=nullptr;
	}
	Array(int n)
	{
		len=n;
		dat=new int [n];
	}
	~Array()
	{
		CleanUp();
	}

	T &operator[](int i)
	{
		return dat[i];
	}
	const T &operator[](int i) const
	{
		return dat[i];
	}
};


int main(int argc,char *argv[])
{
	srand((int)time(nullptr));

	if(2<=argc)
	{
		const int nn=atoi(argv[1]);

		Array<int> ary(nn);
		for(int i=0; i<nn; ++i)
		{
			ary[i]=i;
		}
		Shuffle(nn,ary);

		Array<int> ary2(1);
		ary2=ary;

		for(int i=0; i<nn; ++i)
		{
			printf("$ %d\n",ary2[i]);
		}

	}
	return 0;
}
