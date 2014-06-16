#include "n2DLib.h"

int main(int argc, char *argv[])
{
	if(initBuffering())
	{
		deinitBuffering();
		exit(0);
	}
	
	clearBufferB();
	
	fillCircle(160, 120, 80, 255, 0, 0);
	fillEllipse(160, 120, 70, 60, 0, 0, 255);
	drawPolygon(0, 255, 0, 4, 140, 100, 180, 100, 180, 140, 140, 140);
	
	updateScreen();
	
	
	while(!any_key_pressed());
	
	deinitBuffering();
	
	return 0;
}