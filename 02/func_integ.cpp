#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

class FunctionEvaluator
{
public:
	virtual double Evaluate(double x) const=0;
};

class XSquare : public FunctionEvaluator
{
	virtual double Evaluate(double x) const
	{
		return x*x;
	}
};

class LinearX : public FunctionEvaluator
{
	virtual double Evaluate(double x) const
	{
		return x;
	}
};


double Integrate(const FunctionEvaluator &func,double x0,double x1, double step)
{
	double sum=0.0;
	for(auto x=x0; x<=x1; x+=step)
	{
		sum+=func.Evaluate(x)*step;
	}
	return sum;
}



class ThreadInfo
{
public:
	FunctionEvaluator *funcPtr;
	double x0,x1,step;
	double sum;
};

void ThreadEntry(ThreadInfo *infoPtr)
{
	auto &info=*infoPtr;
	info.sum=Integrate(*info.funcPtr,info.x0,info.x1,info.step);
	printf("Starting a thread\n");
}



int main(void)
{
	char str[256];
	printf(">");
	fgets(str,255,stdin);
   
	FunctionEvaluator *func=nullptr;
	LinearX linear;
	XSquare square;
	switch(atoi(str))
	{
	case 0:
		func=&linear;
		break;
	default:
		func=&square;
		break;
	}

	ThreadInfo info[4];
	std::thread thr[4];
	for(int i=0; i<4; ++i)
	{
		info[i].funcPtr=func;
		info[i].x0=(double)i/4.0;
		info[i].x1=(double)(i+1)/4.0;
		info[i].step=0.0000001;
		std::thread t(ThreadEntry,&info[i]);
		thr[i].swap(t);
	}
	for(auto &t : thr)
	{
		t.join();
	}

	auto sum=info[0].sum+info[1].sum+info[2].sum+info[3].sum;
	printf("%lf\n",sum);
	return 0;
}
