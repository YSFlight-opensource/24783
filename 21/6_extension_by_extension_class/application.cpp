#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>
#include <fslazywindow.h>

#include "application.h"
#include "extension.h"

/* virtual */ void Application::OnInitialize(void)
{
	bgColor[0]=1;
	bgColor[1]=1;
	bgColor[2]=1;
	bgColor[3]=1;

	AddMenu("R-key  Red"  ,FSKEY_R,&Application::SetRedBackground,this);
	AddMenu("G-key  Green",FSKEY_G,&Application::SetGreenBackground,this);
	AddMenu("B-key  Blue", FSKEY_B,&Application::SetBlueBackground,this);

	extPtr=Extension::GetExtension();
	extPtr->SetUpMenu(*this);
}
/* virtual */ void Application::OnDraw(void)
{
	glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	DrawGui();
	FsSwapBuffers();
}
/* virtual */ void Application::OnKeyDown(int fskey)
{
	GuiBase::OnKeyDown(fskey);
}

void Application::SetRedBackground(void)
{
	bgColor[0]=1;
	bgColor[1]=0;
	bgColor[2]=0;
}
void Application::SetGreenBackground(void)
{
	bgColor[0]=0;
	bgColor[1]=1;
	bgColor[2]=0;
}
void Application::SetBlueBackground(void)
{
	bgColor[0]=0;
	bgColor[1]=0;
	bgColor[2]=1;
}
