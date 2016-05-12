#ifndef SPHEREUTIL_IS_INCLUDED
#define SPHEREUTIL_IS_INCLUDED
/* { */

template <class T>
std::vector <T> MakeSphere(int nDiv)
{
	const double YsPi=3.14159265358979323;

	std::vector <T> vtx;
	const int nDivY=nDiv/2;
	const int nDivX=nDiv;

	for(int ip=0; ip<nDivY; ++ip)
	{
		const double p0=-YsPi/2.0+YsPi*(double)ip/(double)(nDivY);
		const double p1=-YsPi/2.0+YsPi*(double)(ip+1)/(double)(nDivY);

		const double y0=sin(p0);
		const double y1=sin(p1);
		const double lateral0=cos(p0);
		const double lateral1=cos(p1);
		for(int ih=0; ih<nDivX; ++ih)
		{
			const double h0=YsPi*2.0*(double)ih/(double)nDivX;
			const double h1=YsPi*2.0*(double)(ih+1)/(double)nDivX;

			const double x0=lateral0*cos(h0);
			const double z0=lateral0*sin(h0);
			const double x1=lateral0*cos(h1);
			const double z1=lateral0*sin(h1);
			const double x2=lateral1*cos(h0);
			const double z2=lateral1*sin(h0);
			const double x3=lateral1*cos(h1);
			const double z3=lateral1*sin(h1);

			const double tri[3*6]=
			{
				x0,y0,z0, x2,y1,z2, x3,y1,z3,
				x3,y1,z3, x1,y0,z1, x0,y0,z0
			};

			if(ip<nDivY-1)
			{
				vtx.push_back((T)tri[0]);
				vtx.push_back((T)tri[1]);
				vtx.push_back((T)tri[2]);
				vtx.push_back((T)tri[3]);
				vtx.push_back((T)tri[4]);
				vtx.push_back((T)tri[5]);
				vtx.push_back((T)tri[6]);
				vtx.push_back((T)tri[7]);
				vtx.push_back((T)tri[8]);
			}
			if(0<ip)
			{
				vtx.push_back((T)tri[ 9]);
				vtx.push_back((T)tri[10]);
				vtx.push_back((T)tri[11]);
				vtx.push_back((T)tri[12]);
				vtx.push_back((T)tri[13]);
				vtx.push_back((T)tri[14]);
				vtx.push_back((T)tri[15]);
				vtx.push_back((T)tri[16]);
				vtx.push_back((T)tri[17]);
			}
		}
	}
	return vtx;
}

/* } */
#endif
