#include <stdlib.h>

#include <ysmergesort.h>

#include "particle.h"

void ParticleGroup::CleanUp(void)
{
	particle.clear();
	vtx.clear();
	col.clear();
	texCoordRange.clear();
}
void ParticleGroup::MakeCloud(int nParticle,const YsVec3 &min,const YsVec3 &max)
{
	particle.resize(nParticle);
	for(auto &p : particle)
	{
		double s=(double)rand()/(double)RAND_MAX;
		double t=(double)rand()/(double)RAND_MAX;
		double u=(double)rand()/(double)RAND_MAX;

		double x=min.x()*(1.0-s)+max.x()*s;
		double y=min.y()*(1.0-t)+max.y()*t;
		double z=min.z()*(1.0-u)+max.z()*u;

		p.pos.Set(x,y,z);

		// Assume 4x4 texture atlas
		s=0.25*(double)(rand()%4);
		t=0.75;
		p.texCoordRange[0]=(float)s;
		p.texCoordRange[1]=(float)t;
		p.texCoordRange[2]=(float)s+0.25;
		p.texCoordRange[3]=(float)t+0.25;
	}
}

void ParticleGroup::MakeVertexBuffer(const YsVec3 &viewDir,float particleSize)
{
	vtx.resize(particle.size()*3);
	col.resize(particle.size()*4);
	pointSize.resize(particle.size());
	texCoordRange.resize(particle.size()*4);

	auto idxBuf=SortIndex(viewDir);
	int idx=0;
	for(auto &sortedIdx : idxBuf)
	{
		auto &p=particle[sortedIdx];

		vtx[idx*3  ]=p.pos.xf();
		vtx[idx*3+1]=p.pos.yf();
		vtx[idx*3+2]=p.pos.zf();

		col[idx*4  ]=1;
		col[idx*4+1]=1;
		col[idx*4+2]=1;
		col[idx*4+3]=0.3;

		pointSize[idx]=particleSize;

		texCoordRange[idx*4  ]=p.texCoordRange[0];
		texCoordRange[idx*4+1]=p.texCoordRange[1];
		texCoordRange[idx*4+2]=p.texCoordRange[2];
		texCoordRange[idx*4+3]=p.texCoordRange[3];

		++idx;
	}
}

std::vector <int> ParticleGroup::SortIndex(const YsVec3 &viewDir)
{
	std::vector <int> idxBuf;
	idxBuf.resize(particle.size());

	std::vector <double> viewDist;
	viewDist.resize(particle.size());

	int idx=0;
	for(auto &p : particle)
	{
		idxBuf[idx]=idx;
		viewDist[idx]=-viewDir*p.pos;
		++idx;
	}

	YsSimpleMergeSort <double,int> (viewDist.size(),viewDist.data(),idxBuf.data());

	return idxBuf;
}
