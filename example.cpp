#include "n2DLib.h"

int main(int argc, char *argv[])
{
	initBuffering();
	
	clearBufferB();
	
	fillCircle(160, 120, 80, 255, 0, 0);
	fillEllipse(160, 120, 70, 60, 0, 0, 255);
	drawPolygon(0, 255, 0, 4, 140, 100, 180, 100, 180, 140, 140, 140);
	
	fillRect(-10, -10, 20, 20, 0xf800);
	fillRect(310, -10, 20, 20, 0x07e0);
	fillRect(310, 230, 20, 20, 0x001f);
	fillRect(-10, 230, 20, 20, 0xffff);
	
	drawString(0, 120, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqsrtuvwxyz\n0123456789<!%$#>", 0x07e0);
	
	updateScreen();
	
	while(!any_key_pressed());
	
	deinitBuffering();
	
	return 0;
}
