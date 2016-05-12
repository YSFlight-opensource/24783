#ifndef EXTENSION_IS_INCLUDED
#define EXTENSION_IS_INCLUDED
/* { */

#include "application.h"

class ExtendedApplication : public Application
{
public:
	virtual void OnInitialize(void);
	void SetWhiteBackground(void);
};

/* } */
#endif
