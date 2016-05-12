#ifndef GUIBASE_IS_INCLUDED
#define GUIBASE_IS_INCLUDED
/* { */

#include <vector>
#include <string>

class GuiBase
{
public:
	class MenuItem
	{
	public:
		std::string msg;
		int fskey;
		GuiBase *owner;
		void (GuiBase::*func)(void);
	};
private:
	std::vector <MenuItem> menu;

public:
	virtual ~GuiBase();

	void AddMenu(const std::string &msg,int fskey,void (GuiBase::*func)(void));
	void DrawGui(void) const;

	virtual void OnInitialize(void);
	virtual void OnDraw(void);
	virtual void OnKeyDown(int fskey);

	static GuiBase *GetGuiApplication(void);
};

/* } */
#endif
