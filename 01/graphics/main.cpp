#include "fssimplewindow.h"

int main(void)
{
	FsOpenWindow(0,0,800,600,1);

	for(;;)
	{
		FsPollDevice();

		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

		glBegin(GL_LINES);
		for(int x=0; x<800; x+=50)
		{
			glVertex2i(x,0);
			glVertex2i(x,600);
		}
		for(int y=0; y<600; y+=50)
		{
			glVertex2i(0,y);
			glVertex2i(800,y);
		}
		glEnd();
		
		FsSwapBuffers();

		FsSleep(25);
	}
	return 0;
}
