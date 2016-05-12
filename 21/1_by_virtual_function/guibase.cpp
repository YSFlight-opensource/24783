#include <ysclass.h>
#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <fslazywindow.h>

#include "guibase.h"

/* virtual */ GuiBase::~GuiBase()
{
}

void GuiBase::AddMenu(const std::string &msg)
{
	MenuItem newMenu;
	newMenu.msg=msg;
	menu.push_back(newMenu);
}

void GuiBase::DrawGui(void) const
{
	YsGLSLBitmapFontRendererClass renderer;
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	renderer.SetViewportDimension(viewport[2],viewport[3]);
	renderer.RequestFontSize(16,24);

	GLfloat col[4]={0,0,0,1};
	renderer.SetUniformColor(col);

	int y=24;
	for(auto &m : menu)
	{
		renderer.DrawString(0,y,m.msg.c_str());
		y+=24;
	}
}

/* virtual */ void GuiBase::OnInitialize(void)
{
}
/* virtual */ void GuiBase::OnDraw(void)
{
}
/* virtual */ void GuiBase::OnKeyDown(int fskey)
{
}
