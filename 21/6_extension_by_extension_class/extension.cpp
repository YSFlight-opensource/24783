#include <fslazywindow.h>

#include "extension.h"

static Extension *extPtr=nullptr;

Extension *Extension::GetExtension(void)
{
	if(nullptr==extPtr)
	{
		extPtr=new Extension;
	}
	return extPtr;
}

void Extension::SetUpMenu(Application &app)
{
	appPtr=&app;
	app.AddMenu("Y-key Yellow",FSKEY_Y,&Extension::SetYellowBackground,this);
}

void Extension::SetYellowBackground(void)
{
	appPtr->bgColor[0]=1;
	appPtr->bgColor[1]=1;
	appPtr->bgColor[2]=0;
}
