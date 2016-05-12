/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp.cpp
Copyright (c) 2015 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <ysclass.h>
#include <ysport.h>

#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <ysshellext.h>
#include <ysshellextio.h>

#include "fsgui3dapp.h"



// If you want to use fixed-function pipeline of OpenGL 1.x,
//   (1) comment out the following line, and
//   (2) change linking libraries in CMakeLists.txt from:
//          fsguilib_gl2 fsgui3d_gl2
//       to
//          fsguilib_gl1 fsgui3d_gl1
#define DONT_USE_FIXED_FUNCTION_PIPELINE



static FsGui3DMainCanvas *appMainCanvas;

////////////////////////////////////////////////////////////

FsGui3DMainCanvas::FsGui3DMainCanvas() : threeDInterface(this),viewControlDlg(NULL)
{
	appMustTerminate=YSFALSE;

	MakeMainMenu();
	AddViewControlDialog();
	drawEnv.AttachCanvas(this);

	bgColor[0]=1;
	bgColor[1]=1;
	bgColor[2]=1;
	bgColor[3]=1;
}

FsGui3DMainCanvas::~FsGui3DMainCanvas()
{
	// The following two lines ensure that all self-destructive dialogs are cleaned. 2015/03/18
	RemoveDialogAll();
	PerformScheduledDeletion();

	DeleteMainMenu();
	if(NULL!=viewControlDlg)
	{
		FsGui3DViewControlDialog::Delete(viewControlDlg);
	}
}

void FsGui3DMainCanvas::LoadModel(const char fn[])
{
	YsString str(fn);
	auto ext=str.GetExtension();

	if(0==ext.STRCMP(".SRF"))
	{
		YsFileIO::File fp(fn,"r");
		if(nullptr!=fp)
		{
			auto inStream=fp.InStream();
			shl.LoadSrf(inStream);
		}
	}
	else if(0==ext.STRCMP(".OBJ"))
	{
		YsFileIO::File fp(fn,"r");
		if(nullptr!=fp)
		{
			auto inStream=fp.InStream();
			shl.LoadObj(inStream);
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
			shl.LoadOff(inStream);
		}
	}

	RemakeVertexAttribArray();
	shl.EnableSearch();
	selectedVertex.clear();
	selVtxBuffer.clear();
}

void FsGui3DMainCanvas::RemakeVertexAttribArray(void)
{
	vtx.clear();
	nom.clear();
	col.clear();
	wireVtx.clear();
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
			for(int i=0; i<plVtHd.GetN(); ++i)
			{
				YsVec3 vtPos[2]=
				{
					shl.GetVertexPosition(plVtHd[i]),
					shl.GetVertexPosition(plVtHd.GetCyclic(i+1))
				};
				wireVtx.push_back(vtPos[0].xf());
				wireVtx.push_back(vtPos[0].yf());
				wireVtx.push_back(vtPos[0].zf());
				wireVtx.push_back(vtPos[1].xf());
				wireVtx.push_back(vtPos[1].yf());
				wireVtx.push_back(vtPos[1].zf());
			}
		}
	}
}

YsShell::VertexHandle FsGui3DMainCanvas::PickedVtHd(int mx,int my) const
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	double pickedZ=YsInfinity;
	YsShell::VertexHandle pickedVtHd=nullptr;
	for(auto vtHd : shl.AllVertex())
	{
		YsVec3 pos=shl.GetVertexPosition(vtHd);
		drawEnv.GetViewMatrix().Mul(pos,pos,1.0);
		drawEnv.GetProjectionMatrix().Mul(pos,pos,1.0);
		if(-1.0<=pos.z() && pos.z()<=1.0)
		{
			const double u=(pos.x()+1.0)/2.0;
			const double v=(pos.y()+1.0)/2.0;

			int x=(int)((double)wid*u);
			int y=hei-(int)((double)hei*v);
			if(mx-8<=x && x<=mx+8 && my-8<=y && y<=my+8)
			{
				if(nullptr==pickedVtHd || pos.z()<pickedZ)
				{
					pickedVtHd=vtHd;
					pickedZ=pos.z();
				}
			}
		}
	}

	return pickedVtHd;
}

void FsGui3DMainCanvas::RemakeSelectedVertexBuffer(void)
{
	selVtxBuffer.clear();
	for(auto vtHd : selectedVertex)
	{
		auto pos=shl.GetVertexPosition(vtHd);
		selVtxBuffer.push_back(pos.xf());
		selVtxBuffer.push_back(pos.yf());
		selVtxBuffer.push_back(pos.zf());
	}
}

void FsGui3DMainCanvas::Initialize(int argc,char *argv[])
{
	YsDisregardVariable(argc);
	YsDisregardVariable(argv);

	YsGLSLCreateSharedRenderer();

	drawEnv.SetTargetBoundingBox(-5.0*YsXYZ(),5.0*YsXYZ());
	drawEnv.SetViewTarget(YsOrigin());
	drawEnv.SetViewDistance(7.5);
}

void FsGui3DMainCanvas::AddViewControlDialog(void)
{
	viewControlDlg=FsGui3DViewControlDialog::Create();
	viewControlDlg->Make(this,&drawEnv);
	this->AddDialog(viewControlDlg);
}

void FsGui3DMainCanvas::MakeMainMenu(void)
{
	mainMenu=new FsGuiPopUpMenu;
	mainMenu->Initialize();
	mainMenu->SetIsPullDownMenu(YSTRUE);

	FsGuiPopUpMenuItem *fileMenu=mainMenu->AddTextItem(0,FSKEY_F,L"File");
	FsGuiPopUpMenu *fileSubMenu=fileMenu->GetSubMenu();
	fileSubMenu->AddTextItem(0,FSKEY_O,L"Open")->BindCallBack(&THISCLASS::File_Open,this);
	fileSubMenu->AddTextItem(0,FSKEY_X,L"Exit")->BindCallBack(&THISCLASS::File_Exit,this);

	auto editSubMenu=mainMenu->AddTextItem(0,FSKEY_E,L"Edit")->GetSubMenu();
	editSubMenu->AddTextItem(0,FSKEY_U,L"Undo")->BindCallBack(&THISCLASS::Edit_Undo,this);
	editSubMenu->AddTextItem(0,FSKEY_R,L"Redo")->BindCallBack(&THISCLASS::Edit_Redo,this);
	editSubMenu->AddTextItem(0,FSKEY_NULL,L"Edge Collapse")->BindCallBack(&THISCLASS::Edit_CollapseEdge,this);
	editSubMenu->AddTextItem(0,FSKEY_R,L"Swap Edge")->BindCallBack(&THISCLASS::Edit_SwapEdge,this);


	auto viewSubMenu=mainMenu->AddTextItem(0,FSKEY_V,L"View")->GetSubMenu();

	wireframe=viewSubMenu->AddTextItem(0,FSKEY_NULL,L"Draw Wireframe");
	wireframe->SetCheck(YSTRUE);
	wireframe->BindCallBack(&THISCLASS::FlipMenuCheck,this);
	lighting=viewSubMenu->AddTextItem(0,FSKEY_NULL,L"Lighting");
	lighting->BindCallBack(&THISCLASS::FlipMenuCheck,this);
	lighting->SetCheck(YSTRUE);

	viewSubMenu->AddTextItem(0,FSKEY_NULL,L"Red Background")->BindCallBack(&THISCLASS::View_SetRedBackground,this);
	viewSubMenu->AddTextItem(0,FSKEY_NULL,L"Green Background")->BindCallBack(&THISCLASS::View_SetGreenBackground,this);
	viewSubMenu->AddTextItem(0,FSKEY_NULL,L"Blue Background")->BindCallBack(&THISCLASS::View_SetBlueBackground,this);
	viewSubMenu->AddTextItem(0,FSKEY_NULL,L"White Background")->BindCallBack(&THISCLASS::View_SetWhiteBackground,this);


	auto selectSubMenu=mainMenu->AddTextItem(0,FSKEY_S,L"Select")->GetSubMenu();
	selectSubMenu->AddTextItem(0,FSKEY_NULL,L"Unselect All")->BindCallBack(&THISCLASS::Select_UnselectAll,this);
	selectSubMenu->AddTextItem(0,FSKEY_NULL,L"Pick Vertices")->BindCallBack(&THISCLASS::Select_Vertex,this);


	SetMainMenu(mainMenu);
}

void FsGui3DMainCanvas::DeleteMainMenu(void)
{
	delete mainMenu;
}

void FsGui3DMainCanvas::OnInterval(void)
{
	FsGuiCanvas::Interval();

	{
		int key;
		while(FSKEY_NULL!=(key=FsInkey()))
		{
			if(YSOK!=drawEnv.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
			{
				if(YSOK!=threeDInterface.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
				{
					this->KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT));
				}
			}
		}
	}

	{
		int charCode;
		while(0!=(charCode=FsInkeyChar()))
		{
			this->CharIn(charCode);
		}
	}

	{
		int lb,mb,rb,mx,my;
		while(FSMOUSEEVENT_NONE!=FsGetMouseEvent(lb,mb,rb,mx,my))
		{
			if(YSOK!=drawEnv.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
			{
				if(YSOK!=this->SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
				{
					if(YSOK!=threeDInterface.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
					{
					}
				}
			}
		}
	}

	{
		auto nTouch=FsGetNumCurrentTouch();
		auto touch=FsGetCurrentTouch();
		if(YSOK!=drawEnv.SetTouchState(nTouch,touch))
		{
			if(YSOK!=this->SetTouchState(nTouch,touch))
			{
				if(YSOK!=threeDInterface.SetTouchState(nTouch,touch))
				{
				}
			}
		}
	}

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	this->SetWindowSize(winWid,winHei,/*autoArrangeDialog=*/YSTRUE);

	drawEnv.SetWindowSize(winWid,winHei);
	drawEnv.SetViewportByTwoCorner(0,0,winWid,winHei);

	if(YSTRUE==drawEnv.NeedRedraw() ||
	   YSTRUE==threeDInterface.NeedRedraw() ||
	   0!=FsCheckWindowExposure())
	{
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::Draw(void)
{
	// Do this at the beginning of Draw funtion.  This will allow one of the elements set SetNeedRedraw(YSTRUE) 
	// within drawing function so that Draw function will be called again in the next iteragion. >>
	SetNeedRedraw(YSFALSE);
	drawEnv.SetNeedRedraw(YSFALSE);
	threeDInterface.SetNeedRedraw(YSFALSE);
	// <<

	int viewport[4];
	drawEnv.GetOpenGlCompatibleViewport(viewport);
	drawEnv.SetVerticalOrientation(YSTRUE);
	drawEnv.UpdateNearFar();


	glUseProgram(0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);


	const YsMatrix4x4 &projMat=drawEnv.GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=drawEnv.GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);

	threeDInterface.SetViewport(viewport);
	threeDInterface.SetProjViewModelMatrix(projViewMat);


	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

	GLfloat glProjMat[16];
	projMat.GetOpenGlCompatibleMatrix(glProjMat);

	GLfloat glModelviewMat[16];
	viewMat.GetOpenGlCompatibleMatrix(glModelviewMat);

#ifndef DONT_USE_FIXED_FUNCTION_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(glProjMat);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(glModelviewMat);
#endif


	YsGLSLSetShared3DRendererProjection(glProjMat);
	YsGLSLSetShared3DRendererModelView(glModelviewMat);



	glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	threeDInterface.Draw();

	if(YSTRUE==lighting->GetCheck())
	{
		YsGLSLShaded3DRenderer renderer;

		GLfloat lightDir[3]={0,0,1};
		renderer.SetLightDirectionInCameraCoordinate(0,lightDir);
		renderer.DrawVtxNomCol(GL_TRIANGLES,vtx.size()/3,vtx.data(),nom.data(),col.data());
	}
	else
	{
		YsGLSLPlain3DRenderer renderer;
		renderer.DrawVtxCol(GL_TRIANGLES,vtx.size()/3,vtx.data(),col.data());
	}

	{
		YsGLSLPlain3DRenderer renderer;
		renderer.SetUniformPointSize(8.0f);
		glEnable(GL_PROGRAM_POINT_SIZE);
		GLfloat magenta[]={1,0,1,1};
		renderer.SetZOffset(-0.0001);
		renderer.SetUniformColor(magenta);
		renderer.DrawVtx(GL_POINTS,selVtxBuffer.size()/3,selVtxBuffer.data());
		glDisable(GL_PROGRAM_POINT_SIZE);

		if(YSTRUE==wireframe->GetCheck())
		{
			GLfloat black[]={0,0,0,1};
			renderer.SetUniformColor(black);
			renderer.DrawVtx(GL_LINES,wireVtx.size()/3,wireVtx.data());
		}
		renderer.SetZOffset(0);
	}


	int nTri=0;
	YsVec3 tri[9];
	if(YSOK==FsGui3DGetArrowHeadTriangle(
	    tri+nTri*3,YsOrigin(),YsXVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		++nTri;
	}
	if(YSOK==FsGui3DGetArrowHeadTriangle(
	    tri+nTri*3,YsOrigin(),YsYVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		++nTri;
	}
	if(YSOK==FsGui3DGetArrowHeadTriangle(
	    tri+nTri*3,YsOrigin(),YsZVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		++nTri;
	}

#ifdef DONT_USE_FIXED_FUNCTION_PIPELINE
	{
        auto renderer=YsGLSLSharedVariColor3DRenderer();
        YsGLSLUse3DRenderer(renderer);

		const float col[4]={0,0,0,1};
        YsGLSLSet3DRendererUniformColorfv(renderer,col);

        const float vtxPos[6*3]=
		{
			0,0,0, 1,0,0,
			0,0,0, 0,1,0,
			0,0,0, 0,0,1
		};
        YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,6,vtxPos);

		if(0<nTri)
		{
	        float triVtxPos[9*3];
			for(int i=0; i<nTri*3; ++i)
			{
				triVtxPos[i*3  ]=tri[i].xf();
				triVtxPos[i*3+1]=tri[i].yf();
				triVtxPos[i*3+2]=tri[i].zf();
			}
	        YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,nTri*3,triVtxPos);
		}
        YsGLSLEndUse3DRenderer(renderer);
	}
#else
	glUseProgram(0);
	glColor3ub(0,0,0);
	glBegin(GL_LINES);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(1.0,0.0,0.0);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(0.0,1.0,0.0);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(0.0,0.0,1.0);
	glEnd();

	if(0<nTri)
	{
		glBegin(GL_TRIANGLES);
		for(int i=0; i<nTri*3; ++i)
		{
			glVertex3dv(tri[i]);
		}
		glEnd();
	}
#endif


	// 2D Drawing
#ifndef DONT_USE_FIXED_FUNCTION_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(double)viewport[2],(double)viewport[3],0.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	glDisable(GL_DEPTH_TEST);

	glUseProgram(0);
	FsGuiCanvas::Show();

	FsSwapBuffers();
}



////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::FlipMenuCheck(FsGuiPopUpMenuItem *menu)
{
	auto check=menu->GetCheck();
	YsFlip(check);
	menu->SetCheck(check);
	SetNeedRedraw(YSTRUE);
}

void FsGui3DMainCanvas::File_Exit(FsGuiPopUpMenuItem *)
{
	auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	msgDlg->Make(L"Confirm Exit?",L"Confirm Exit?",L"Yes",L"No");
	msgDlg->BindCloseModalCallBack(&THISCLASS::File_Exit_ConfirmExitCallBack,this);
	AttachModalDialog(msgDlg);
}

void FsGui3DMainCanvas::File_Exit_ConfirmExitCallBack(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		File_Exit_ReallyExit();
	}
}

void FsGui3DMainCanvas::File_Exit_ReallyExit(void)
{
	this->appMustTerminate=YSTRUE;
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::Edit_Undo(FsGuiPopUpMenuItem *)
{
	shl.Undo();
	RemakeVertexAttribArray();
}
void FsGui3DMainCanvas::Edit_Redo(FsGuiPopUpMenuItem *)
{
	shl.Redo();
	RemakeVertexAttribArray();
}

void FsGui3DMainCanvas::Edit_CollapseEdge(FsGuiPopUpMenuItem *)
{
	YsShellExtEdit::StopIncUndo undoGuard(shl);
	if(2==selectedVertex.size())
	{
		// Triangles using the edge must be deleted.
		for(auto plHd : shl.FindPolygonFromEdgePiece(selectedVertex[0],selectedVertex[1]))
		{
			shl.DeletePolygon(plHd);
		}
		// Reconnect selectedVertex[0] to selectedVertex[1]
		for(auto plHd : shl.FindPolygonFromVertex(selectedVertex[0]))
		{
			auto plVtHd=shl.GetPolygonVertex(plHd);
			for(auto &vtHd : plVtHd)
			{
				if(selectedVertex[0]==vtHd)
				{
					vtHd=selectedVertex[1];
				}
			}
			shl.SetPolygonVertex(plHd,plVtHd);
		}
		RemakeVertexAttribArray();
		SetNeedRedraw(YSTRUE);
	}
	else
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		dlg->Make(L"Error",L"Select 2 vertices and try again.",L"YSOK",nullptr);
		AttachModalDialog(dlg);
	}
}

void FsGui3DMainCanvas::Edit_SwapEdge(FsGuiPopUpMenuItem *)
{
	YsShellExtEdit::StopIncUndo undoGuard(shl);
	if(2==selectedVertex.size())
	{
		YsShell::VertexHandle edVtHd[2]={selectedVertex[0],selectedVertex[1]};

		auto edPlHd=shl.FindPolygonFromEdgePiece(edVtHd[0],edVtHd[1]);
		if(2==edPlHd.GetN() &&
		   3==shl.GetPolygonNumVertex(edPlHd[0]) &&
		   3==shl.GetPolygonNumVertex(edPlHd[1]))
		{
			auto plVtHd0=shl.GetPolygonVertex(edPlHd[0]);
			auto plVtHd1=shl.GetPolygonVertex(edPlHd[1]);

			// First merge two polygons into a quadrilateral.
			int edIdxInPlg0=-1;
			for(int i=0; i<3; ++i)
			{
				if((plVtHd0[i]==edVtHd[0] && plVtHd0.GetCyclic(i+1)==edVtHd[1]) ||
				   (plVtHd0[i]==edVtHd[1] && plVtHd0.GetCyclic(i+1)==edVtHd[0]))
				{
					edIdxInPlg0=i;
					break;
				}
			}
			// Find which vertex should be merged.
			YsShell::VertexHandle vtHdToMerge=nullptr;
			for(int i=0; i<3; ++i)
			{
				if(plVtHd1[i]!=edVtHd[0] && plVtHd1[i]!=edVtHd[1])
				{
					vtHdToMerge=plVtHd1[i];
					break;
				}
			}

			if(nullptr!=vtHdToMerge && 0<=edIdxInPlg0)
			{
				plVtHd0.Insert(edIdxInPlg0+1,vtHdToMerge);
			}

			YsShell::VertexHandle triVtHd[2][3]=
			{
				{
					plVtHd0.GetCyclic(edIdxInPlg0+1),
					plVtHd0.GetCyclic(edIdxInPlg0+2),
					plVtHd0.GetCyclic(edIdxInPlg0+3)
				},
				{
					plVtHd0.GetCyclic(edIdxInPlg0+3),
					plVtHd0.GetCyclic(edIdxInPlg0+0),
					plVtHd0.GetCyclic(edIdxInPlg0+1)
				}
			};

			shl.SetPolygonVertex(edPlHd[0],3,triVtHd[0]);
			shl.SetPolygonVertex(edPlHd[1],3,triVtHd[1]);

			RemakeVertexAttribArray();
			SetNeedRedraw(YSTRUE);
		}
	}
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::View_SetRedBackground(FsGuiPopUpMenuItem *)
{
	bgColor[0]=1;
	bgColor[1]=0;
	bgColor[2]=0;
	bgColor[3]=1;
}
void FsGui3DMainCanvas::View_SetBlueBackground(FsGuiPopUpMenuItem *)
{
	bgColor[0]=0;
	bgColor[1]=1;
	bgColor[2]=0;
	bgColor[3]=1;
}
void FsGui3DMainCanvas::View_SetGreenBackground(FsGuiPopUpMenuItem *)
{
	bgColor[0]=0;
	bgColor[1]=0;
	bgColor[2]=1;
	bgColor[3]=1;
}
void FsGui3DMainCanvas::View_SetWhiteBackground(FsGuiPopUpMenuItem *)
{
	bgColor[0]=1;
	bgColor[1]=1;
	bgColor[2]=1;
	bgColor[3]=1;
}


void FsGui3DMainCanvas::Select_UnselectAll(FsGuiPopUpMenuItem *)
{
	selectedVertex.clear();
	RemakeSelectedVertexBuffer();
}

void FsGui3DMainCanvas::Select_Vertex(FsGuiPopUpMenuItem *)
{
	BindLButtonDownCallBack(&THISCLASS::Select_Vertex_LButtonDown,this);
}
YSRESULT FsGui3DMainCanvas::Select_Vertex_LButtonDown(FsGuiMouseButtonSet,YsVec2i mos)
{
	auto vtHd=PickedVtHd(mos.x(),mos.y());
	if(nullptr!=vtHd)
	{
		for(auto v : selectedVertex)
		{
			if(v==vtHd)
			{
				return YSOK;
			}
		}
		selectedVertex.push_back(vtHd);
		RemakeSelectedVertexBuffer();
		SetNeedRedraw(YSTRUE);
	}
	return YSOK;
}
