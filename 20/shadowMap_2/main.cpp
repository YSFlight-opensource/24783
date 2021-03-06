#include <stdio.h>
#include <ysclass.h>
#include <ysport.h>

#include <ysshellext.h>
#include <ysshellextio.h>

#include <fslazywindow.h>
#include "renderer.h"
#include "shadow_map_buffer.h"

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	ColorAsAttributeRenderer colorAsAttrib;
	UniformColorRenderer uniformColor;
	Plain3dRenderer plain3d;
	Rainbow3dRenderer rainbow3d;
	Gouraud3dRenderer gouraud3d;
	Phong3dRenderer phong3d;
	DepthVerifyRenderer depthVerify;

	OpenGLShadowMapBuffer shadowMapBuf;

	// Cut & Pasted from stl viewer >>
	Ys3DDrawingEnvironment drawEnv;

	int prevMx,prevMy;
	YsShellExt shl;
	std::vector <float> vtx,nom;
	std::vector <float> col;
	YsVec3 min,max;

	YsVec3 lightDir;
	mutable YsMatrix4x4 lightViewTfm,lightProjTfm;

	void LoadModel(const char fn[]);
	void CacheBoundingBox(void);
	static void YsShellToVtxNom(std::vector <float> &vtx,std::vector <float> &nom,std::vector <float> &col,const YsShellExt &shl);

	void RenderShadowBuffer(void) const;
	void VerifyShadowMap(void) const;
	// Cut & Pasted from stl viewer <<


public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

void FsLazyWindowApplication::LoadModel(const char fn[])
{
	YsString str(fn);
	auto ext=str.GetExtension();

	if(0==ext.STRCMP(".SRF"))
	{
		YsFileIO::File fp(fn,"r");
		if(nullptr!=fp)
		{
			auto inStream=fp.InStream();
			YsShellExtReader reader;
			reader.MergeSrf(shl,inStream);
		}
	}
	else if(0==ext.STRCMP(".OBJ"))
	{
		YsFileIO::File fp(fn,"r");
		if(nullptr!=fp)
		{
			auto inStream=fp.InStream();
			YsShellExtObjReader reader;
			YsShellExtObjReader::ReadOption defaultOption;
			reader.ReadObj(shl,inStream,defaultOption);
		}
	}
	else if(0==ext.STRCMP(".STL"))
	{
		shl.LoadStl(fn);
		for(auto plHd : shl.AllPolygon())
		{
			shl.SetPolygonColor(plHd,YsBlue());
		}
	}
	else if(0==ext.STRCMP(".OFF"))
	{
		YsFileIO::File fp(fn,"r");
		if(nullptr!=fp)
		{
			auto inStream=fp.InStream();
			YsShellExtOffReader reader;
			reader.ReadOff(shl,inStream);
		}
	}

	YsShellToVtxNom(vtx,nom,col,shl);
	shl.EnableSearch();
	CacheBoundingBox();
}

void FsLazyWindowApplication::CacheBoundingBox(void)
{
	shl.GetBoundingBox(min,max);
}

/* static */ void FsLazyWindowApplication::YsShellToVtxNom(std::vector <float> &vtx,std::vector <float> &nom,std::vector <float> &col,const YsShellExt &shl)
{
	vtx.clear();
	nom.clear();
	col.clear();
	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		if(3<=plVtHd.GetN())
		{
			auto plNom=shl.GetNormal(plHd);
			auto plCol=shl.GetColor(plHd);
			for(auto vtHd : plVtHd)
			{
				auto vtPos=shl.GetVertexPosition(vtHd);
				vtx.push_back(vtPos.xf());
				vtx.push_back(vtPos.yf());
				vtx.push_back(vtPos.zf());
				nom.push_back(plNom.xf());
				nom.push_back(plNom.yf());
				nom.push_back(plNom.zf());
				col.push_back(plCol.Rf());
				col.push_back(plCol.Gf());
				col.push_back(plCol.Bf());
				col.push_back(1);
			}
		}
	}
}

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;

	// Cut & Pasted from stl viewer >>
	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetViewTarget(YsVec3::Origin());
	drawEnv.SetViewDistance(20.0);
	drawEnv.SetViewAttitude(YsAtt3(YsPi/4.0,YsPi/5.0,0.0));

	prevMx=0;
	prevMy=0;

	needRedraw=false;
	// Cut & Pasted from stl viewer <<
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
	// Cut & Pasted from stl viewer >>
	if(2<=argc)
	{
		LoadModel(argv[1]);
	}
	// Cut & Pasted from stl viewer <<
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	FsChangeToProgramDir();
	colorAsAttrib.CompileFile(
	    "color_as_attribute_vertex_shader.glsl",
	    "color_as_attribute_fragment_shader.glsl");
	uniformColor.CompileFile(
	    "uniform_color_vertex_shader.glsl",
	    "uniform_color_fragment_shader.glsl");
	plain3d.CompileFile(
		"plain3d_vertex_shader.glsl",
		"plain3d_fragment_shader.glsl");
	rainbow3d.CompileFile(
		"rainbow3d_vertex_shader.glsl",
		"rainbow3d_fragment_shader.glsl");
	gouraud3d.CompileFile(
		"gouraud_vertex_shader.glsl",
		"gouraud_fragment_shader.glsl");
	phong3d.CompileFile(
		"phong_vertex_shader.glsl",
		"phong_fragment_shader.glsl");
	depthVerify.CompileFile(
		"depth_verify_vertex_shader.glsl",
		"depth_verify_fragment_shader.glsl");

	shadowMapBuf.PrepareBuffer(1024,1024);
	lightDir=YsVec3(0,1,1);
	lightDir.Normalize();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	// Cut & Pasted from stl viewer >>
	int wid,hei;
	FsGetWindowSize(wid,hei);

	int lb,mb,rb,mx,my;
	auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(0!=lb && (mx!=prevMx || my!=prevMy))
	{
		double denom=(double)YsGreater(wid,hei);
		double dx=2.0*(double)(prevMx-mx)/denom;
		double dy=2.0*(double)(prevMy-my)/denom;
		drawEnv.RotateView(dx,dy);
	}
	prevMx=mx;
	prevMy=my;
	// Cut & Pasted from stl viewer <<

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	RenderShadowBuffer();

	int wid,hei;
	FsGetWindowSize(wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	drawEnv.SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	drawEnv.SetAspectRatio((double)wid/(double)hei);
	drawEnv.SetFOVY(YsPi/4.0);
	drawEnv.SetNearFar(0.1,100.0);
	drawEnv.SetViewTarget((min+max)/2.0);
	drawEnv.SetViewDistance((max-min).GetLength()/2.0);

	GLfloat projMat[16];
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);

	GLfloat viewMat[16];
	drawEnv.GetViewMatrix().GetOpenGlCompatibleMatrix(viewMat);


	glUseProgram(phong3d.programIdent);
	glUniformMatrix4fv(phong3d.uniformProjectionPos,1,GL_FALSE,projMat);
	glUniformMatrix4fv(phong3d.uniformModelViewPos,1,GL_FALSE,viewMat);

	YsVec3 lightDir;
	drawEnv.GetViewMatrix().Mul(lightDir,this->lightDir,0.0);
	glUniform3f(phong3d.uniformLightDirPos,lightDir.xf(),lightDir.yf(),lightDir.zf());

	glUniform1f(phong3d.uniformAmbientPos,0.3f);
	glUniform1f(phong3d.uniformSpecularIntensityPos,1);
	glUniform1f(phong3d.uniformSpecularExponentPos,100.0f);



	glEnableVertexAttribArray(phong3d.attribVertexPos);
	glEnableVertexAttribArray(phong3d.attribNormalPos);
	glEnableVertexAttribArray(phong3d.attribColorPos);

	glVertexAttribPointer(phong3d.attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx.data());
	glVertexAttribPointer(phong3d.attribNormalPos,3,GL_FLOAT,GL_FALSE,0,nom.data());
	glVertexAttribPointer(phong3d.attribColorPos,4,GL_FLOAT,GL_FALSE,0,col.data());

	glDrawArrays(GL_TRIANGLES,0,vtx.size()/3);

	glDisableVertexAttribArray(phong3d.attribVertexPos);
	glDisableVertexAttribArray(phong3d.attribNormalPos);
	glDisableVertexAttribArray(phong3d.attribColorPos);

	VerifyShadowMap();

	FsSwapBuffers();
	needRedraw=false;
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
}


static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}


void FsLazyWindowApplication::RenderShadowBuffer(void) const
{
	glBindTexture(GL_TEXTURE_2D,0);  // This is needed in MacOSX.
	                                 // It actually makes sense.  Unless the texture is unbound before being written,
	                                 // GPU may still be using it for drawing a fragment, and as a result,
	                                 // drawing to the texture may fail.  It was exactly what was happening, and
	                                 // glClear could clear only a part of the texture unless the texture is unbound.

    int curFrameBuffer=0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING,&curFrameBuffer);
    
	glBindFramebuffer(GL_FRAMEBUFFER,shadowMapBuf.frameBufferIdent);

	YsAtt3 viewAtt(0,0,0);
	viewAtt.SetForwardVector(-lightDir);

	Ys3DDrawingEnvironment drawEnv;

	int wid,hei;
	FsGetWindowSize(wid,hei);
	drawEnv.SetProjectionMode(Ys3DDrawingEnvironment::ORTHOGONAL);
	drawEnv.SetAspectRatio(1.0);
	drawEnv.SetOrthogonalProjectionHeight(10.0);
	drawEnv.SetNearFar(8.0,100.0);

	drawEnv.SetViewTarget(YsOrigin());
	drawEnv.SetViewDistance(40.0);

	drawEnv.SetViewAttitude(viewAtt);

	glViewport(0,0,1024,1024);

	float viewTfm[16],projTfm[16];
	drawEnv.GetViewMatrix().GetOpenGlCompatibleMatrix(viewTfm);
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projTfm);


	lightViewTfm=drawEnv.GetViewMatrix();
	lightProjTfm=drawEnv.GetProjectionMatrix();



	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);



	GLfloat projMat[16];
	drawEnv.GetProjectionMatrix().GetOpenGlCompatibleMatrix(projMat);

	GLfloat viewMat[16];
	drawEnv.GetViewMatrix().GetOpenGlCompatibleMatrix(viewMat);

	glUseProgram(plain3d.programIdent);
	glUniformMatrix4fv(plain3d.uniformProjectionPos,1,GL_FALSE,projMat);
	glUniformMatrix4fv(plain3d.uniformModelViewPos,1,GL_FALSE,viewMat);

	glEnableVertexAttribArray(plain3d.attribVertexPos);
	glEnableVertexAttribArray(plain3d.attribColorPos);

	glVertexAttribPointer(plain3d.attribVertexPos,3,GL_FLOAT,GL_FALSE,0,vtx.data());
	glVertexAttribPointer(plain3d.attribColorPos,4,GL_FLOAT,GL_FALSE,0,col.data());

	glDrawArrays(GL_TRIANGLES,0,vtx.size()/3);

	glDisableVertexAttribArray(plain3d.attribVertexPos);
	glDisableVertexAttribArray(plain3d.attribColorPos);

	// The color implies that the depth values written in the texture is scaled to 0 to 1, 0 is nearz and 1 is farz.
	// The projection matrix of OpenGL transforms screen z as -1 for nearz and +1 for far z.

	glBindFramebuffer(GL_FRAMEBUFFER,curFrameBuffer);
}

void FsLazyWindowApplication::VerifyShadowMap(void) const
{
	glDisable(GL_DEPTH_TEST);

	const float rectVtx[]=
	{
		-1   ,-1,
		-0.3f,-1,
		-0.3f,-0.3f,
		-1   ,-0.3f
	};
	const float rectTexCoord[]=
	{
		0,0,
		1,0,
		1,1,
		0,1
	};

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,shadowMapBuf.shadowTexIdent);

	glUseProgram(depthVerify.programIdent);

	glEnableVertexAttribArray(depthVerify.attribVertexPos);
	glEnableVertexAttribArray(depthVerify.attribTexCoordPos);

	glUniform1i(depthVerify.uniformTexturePos,0);
	glVertexAttribPointer(depthVerify.attribVertexPos,2,GL_FLOAT,GL_FALSE,0,rectVtx);
	glVertexAttribPointer(depthVerify.attribTexCoordPos,2,GL_FLOAT,GL_FALSE,0,rectTexCoord);
	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisableVertexAttribArray(depthVerify.attribVertexPos);
	glDisableVertexAttribArray(depthVerify.attribTexCoordPos);
}
