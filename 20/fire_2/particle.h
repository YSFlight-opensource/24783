#ifndef PARTICLE_IS_INCLUDED
#define PARTICLE_IS_INCLUDED
/* { */

#include <vector>

#include <ysclass.h>

/*
ParticleGroup:
  1: Store particles.
  2: Generate particles for a cloud.
  3: Generate vertex attribute buffers for rendering.
*/
class ParticleGroup
{
public:
	class Particle
	{
	public:
		YsVec3 pos;
		float texCoordRange[4];

		YsVec3 vel;
		double t,tRemain;
	};

	std::vector <Particle> particle;
	std::vector <float> vtx,pointSize,col,texCoordRange;

	void CleanUp(void);
	void MakeCloud(int nParticle,const YsVec3 &min,const YsVec3 &max);
	void MakeVertexBuffer(const YsVec3 &viewDir,float particleSize);

	void AddFireParticle(int nParticle);
	void Move(const double dt);

	std::vector <int> SortIndex(const YsVec3 &viewDir);
};

/* } */
#endif
