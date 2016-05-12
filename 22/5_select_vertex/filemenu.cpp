#include <stdio.h>

#include <ysport.h>

#include <fsguifiledialog.h>
#include "fsgui3dapp.h"

void FsGui3DMainCanvas::File_Open(FsGuiPopUpMenuItem *)
{
	printf("%s %d\n",__FUNCTION__,__LINE__);

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Open a mesh");
	fdlg->fileExtensionArray.Append(L".stl");
	fdlg->fileExtensionArray.Append(L".srf");
	fdlg->fileExtensionArray.Append(L".sff");
	fdlg->fileExtensionArray.Append(L".obj");
	fdlg->defaultFileName=L"./*.stl";
	fdlg->SetCloseModalCallBack(NULL);
	fdlg->BindCloseModalCallBack(&THISCLASS::File_Open_OnCloseDialog,this);
	this->AttachModalDialog(fdlg);
}

void FsGui3DMainCanvas::File_Open_OnCloseDialog(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode)
	{
		auto fn=fdlg->selectedFileArray[0];

		YsString sysEnc;
		YsUnicodeToSystemEncoding(sysEnc,fn);

		printf("%s\n",(const char *)sysEnc);

		LoadModel(sysEnc);
		YsShellToVtxNom(vtx,nom,col,shl);

		YsVec3 bbx[2];
		shl.GetBoundingBox(bbx);
		drawEnv.SetViewTarget((bbx[0]+bbx[1])/2.0);
		drawEnv.SetTargetBoundingBox(bbx);
	}
}

