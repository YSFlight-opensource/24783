#include "application.h"

static Application *appPtr=nullptr;

/* static */ GuiBase *GuiBase::GetGuiApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new Application;
	}
	return appPtr;
}
