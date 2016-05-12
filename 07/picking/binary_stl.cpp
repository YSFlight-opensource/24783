#include "binary_stl.h"


bool CPUisLittleEndian(void)
{
	unsigned int one=1;
	auto *dat=(const unsigned char *)&one;
	if(1==dat[0])
	{
		return true;
	}
	return false;
}

bool IsAsciiStl(const char fn[])
{
	// You can only make a guess.
	// Does this have keywords "solid" "facet" "loop" and "vertex"?

	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		char buf[1024];
		fread(buf,1,1024,fp);
		fclose(fp);

		bool solid,facet,loop,vertex;
		solid=false;
		facet=false;
		loop=false;
		vertex=false;
		for(int i=0; i<1018; i++)
		{
			if(strncmp(buf+i,"solid",5)==0)
			{
				solid=true;
			}
			else if(strncmp(buf+i,"facet",5)==0)
			{
				facet=true;
			}
			else if(strncmp(buf+i,"loop",4)==0)
			{
				loop=true;
			}
			else if(strncmp(buf+i,"vertex",6)==0)
			{
				vertex=true;
			}
		}

		if(true==solid && true==facet && true==loop && true==vertex)
		{
			return true;
		}
	}
	return false;
}

int BinaryToInt(const unsigned char dw[4])
{
	int b0=(int)dw[0];
	int b1=(int)dw[1];
	int b2=(int)dw[2];
	int b3=(int)dw[3];
	return b0+b1*0x100+b2*0x10000+b3*0x1000000;
}
float BinaryToFloat(const unsigned char dw[4])
{
	if(true==CPUisLittleEndian())
	{
		const float *fPtr=(const float *)dw;
		return *fPtr;
	}
	else
	{
		float value;
		auto *valuePtr=(unsigned char *)&value;
		valuePtr[0]=dw[3];
		valuePtr[1]=dw[2];
		valuePtr[2]=dw[1];
		valuePtr[3]=dw[0];
		return value;
	}
}

void AddBinaryStlTriangle(std::vector <float> &vtx,std::vector <float> &nom,const unsigned char buf[50])
{
	float nx=BinaryToFloat(buf),ny=BinaryToFloat(buf+4),nz=BinaryToFloat(buf+8);
	nom.push_back(nx);
	nom.push_back(ny);
	nom.push_back(nz);
	nom.push_back(nx);
	nom.push_back(ny);
	nom.push_back(nz);
	nom.push_back(nx);
	nom.push_back(ny);
	nom.push_back(nz);

	vtx.push_back(BinaryToFloat(buf+12));
	vtx.push_back(BinaryToFloat(buf+16));
	vtx.push_back(BinaryToFloat(buf+20));
	vtx.push_back(BinaryToFloat(buf+24));
	vtx.push_back(BinaryToFloat(buf+28));
	vtx.push_back(BinaryToFloat(buf+32));
	vtx.push_back(BinaryToFloat(buf+36));
	vtx.push_back(BinaryToFloat(buf+40));
	vtx.push_back(BinaryToFloat(buf+44));

	// buf[48] and buf[49] are volume identifier, which is usually not used.
}

void LoadBinaryStl(std::vector <float> &vtx,std::vector <float> &nom,const char fn[])
{
	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		unsigned char title[80];
		fread(title,1,80,fp); // Skip title

		unsigned char dw[4];
		fread(dw,4,1,fp);  // Read 4 bytes
		auto nTri=BinaryToInt(dw);
		printf("%d triangles\n",nTri);

		int nTriActual=0;
		vtx.clear();
		nom.clear();
		for(int i=0; i<nTri; ++i)
		{
			unsigned char buf[50];  // 50 bytes per triangle
			if(50==fread(buf,1,50,fp))
			{
				AddBinaryStlTriangle(vtx,nom,buf);
				++nTriActual;
			}
			else
			{
				break;
			}
		}
		printf("Actually read %d\n",nTriActual);

		fclose(fp);
	}
}

