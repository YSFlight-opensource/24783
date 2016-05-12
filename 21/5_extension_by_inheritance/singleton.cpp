#include "extension.h"

static ExtendedApplication *appPtr=nullptr;

/* static */ GuiBase *GuiBase::GetGuiApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new ExtendedApplication;
	}
	return appPtr;
}
