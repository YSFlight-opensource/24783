#ifndef APPLICATION_IS_INCLUDED
#define APPLICATION_IS_INCLUDED
/* { */

#include "guibase.h"

class Application : public GuiBase
{
friend class Extension;

public:
	class Extension *extPtr;
	float bgColor[4];

	virtual void OnInitialize(void);
	virtual void OnDraw(void);
	virtual void OnKeyDown(int fskey);

	void SetRedBackground(void);
	void SetGreenBackground(void);
	void SetBlueBackground(void);
};

/* } */
#endif
