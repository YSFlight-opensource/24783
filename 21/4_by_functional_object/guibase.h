#ifndef GUIBASE_IS_INCLUDED
#define GUIBASE_IS_INCLUDED
/* { */

#include <vector>
#include <string>
#include <functional>

class GuiBase
{
public:
	class MenuItem
	{
	public:
		std::string msg;
		int fskey;
		std::function <void (void)> func;
	};
private:
	std::vector <MenuItem> menu;

public:
	virtual ~GuiBase();

	template <class T>
	void AddMenu(const std::string &msg,int fskey,void (T::*func)(void),T *owner)
	{
		MenuItem m;
		m.msg=msg;
		m.fskey=fskey;
		m.func=std::bind(func,owner);
		menu.push_back(m);
	}

	void DrawGui(void) const;

	virtual void OnInitialize(void);
	virtual void OnDraw(void);
	virtual void OnKeyDown(int fskey);

	static GuiBase *GetGuiApplication(void);
};

/* } */
#endif
