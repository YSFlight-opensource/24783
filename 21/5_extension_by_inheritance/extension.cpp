#include <fslazywindow.h>
#include "extension.h"

void ExtendedApplication::OnInitialize(void)
{
	Application::OnInitialize();
	AddMenu("W-key White",FSKEY_W,&ExtendedApplication::SetWhiteBackground,this);
}

void ExtendedApplication::SetWhiteBackground(void)
{
	bgColor[0]=1;
	bgColor[1]=1;
	bgColor[2]=1;
}
