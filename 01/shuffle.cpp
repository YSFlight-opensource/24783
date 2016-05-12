#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Shuffle(int n,int dat[])
{
	for(int i=0; i<n; ++i)
	{
		const int r=rand()%n;
		auto c=dat[i];
		dat[i]=dat[r];
		dat[r]=c;
	}
}

int main(void)
{
	int dat[16];
	for(int i=0; i<16; ++i)
	{
		dat[i]=i;
	}
	Shuffle(16,dat);

	for(auto d : dat)
	{
		printf("%d ",d);
	}
	printf("\n");
	return 0;
}

