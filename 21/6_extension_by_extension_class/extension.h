#ifndef EXTENSION_IS_INCLUDED
#define EXTENSION_IS_INCLUDED
/* { */

#include "application.h"

class Extension
{
public:
	Application *appPtr;
	static Extension *GetExtension(void);

	void SetUpMenu(Application &app);
	void SetYellowBackground(void);
};

/* } */
#endif
